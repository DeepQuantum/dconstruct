#include "decompilation/decomp_function.h"
#include <atomic>
#include <condition_variable>
#include <optional>
#include <stop_token>
#include <windows.h>
#include <DbgEng.h>
#include <wrl/client.h>
#include <print>
#include <format>
#include <cstdlib>
#include <string>
#include <string_view>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <expected>
#include <mutex>
#include <thread>

#ifdef NEAR
#undef NEAR
#endif

#ifdef FAR
#undef FAR
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#ifdef DC_VERSION
#undef DC_VERSION
#endif

#include "disassembly/instructions.h"
#include "ast/function_definition.h"
#include "sidbase.h"

#include "DCScript.h"

#define CHECKED_THREAD(expr, message) if (FAILED(expr)) { return message; }

using namespace std::literals;
using namespace Microsoft;

namespace dconstruct::debugger {

static constexpr ULONG64 PARSE_INSTRUCTION_LOOP_ENTRY = 0x14B6B10;
static constexpr ULONG64 SID_LOCATION_STACK_POINTER_OFFSET = 0x338;

struct debugger_snapshot {
    std::shared_ptr<function_disassembly> m_func;
    std::shared_ptr<ast::function_definition> m_decomp;
    std::array<u64, ARGUMENT_REGISTERS_IDX> m_generalPurposeRegisters{};
    std::array<u64, ARGUMENT_REGISTERS_IDX> m_argumentRegisters{};
    sid64 m_sid = 0;
    u64 m_scriptLambdaPtr = 0;
    u64 m_stackFramePtr = 0;
    ULONG m_debugThreadId = 0;
    u64 m_instructionIdx = 0;
};

class debugger {

#undef ERROR

public:
    enum class STATE : u8 {
        DETACHED,
        ATTACHING,
        ATTACHED,
        SNAPSHOT_READY,
        ERROR
    };

    enum class COMMAND : u8 {
        NONE,
        SINGLE_STEP,
        CONTINUE
    };

    [[nodiscard]] std::optional<std::string> request_attach();
    void detach();

    debugger() {
        m_sink.set_output(&m_output);
    }


    [[nodiscard]] const std::string& output() const noexcept {
        return m_output;
    }

    [[nodiscard]] bool is_attached() const noexcept {
        const STATE state = poll_state();
        return state == STATE::ATTACHED || state == STATE::SNAPSHOT_READY;
    }

    void append_output(std::string_view text) {
        m_output.append(text);
    }

    template<typename sid_iter>
    void set_sids(sid_iter begin, sid_iter end) {
        for (auto i = begin; i < end; ++i) {
            m_toMatchSIDs.push_back(*i);
        }
    }

    void set_sidbase(const SIDBase* sidbase) noexcept {
        if (sidbase != nullptr) {
            m_sidbase = sidbase;
        }
    }

    [[nodiscard]] STATE poll_state() const {
        return m_state.load(std::memory_order_acquire);
    }

    void request_single_step();
    void request_continue();

    [[nodiscard]] std::shared_ptr<debugger_snapshot> poll_snapshot();
    [[nodiscard]] std::optional<std::string> poll_error();


    debugger(debugger&) = delete;
    debugger(debugger&&) = delete;

    debugger& operator=(debugger&) = delete;
    debugger& operator=(debugger&&) = delete;

    ~debugger() {
        detach();
    }

private:
    void loop(std::stop_token st);

    [[nodiscard]] std::optional<std::string> attach();
    void cleanup_session();

    struct OutputSink : IDebugOutputCallbacks {
        void set_output(std::string* output) noexcept {
            m_output = output;
        }

        STDMETHOD(QueryInterface)(REFIID iid, void** obj) override {
            if (iid == __uuidof(IUnknown) || iid == __uuidof(IDebugOutputCallbacks)) {
                *obj = this;
                return S_OK;
            }
            *obj = nullptr;
            return E_NOINTERFACE;
        }
        STDMETHOD_(ULONG, AddRef)() override { return 1; }
        STDMETHOD_(ULONG, Release)() override { return 1; }
        STDMETHOD(Output)(ULONG, PCSTR text) override {
            if (m_output != nullptr && text != nullptr) {
                m_output->append(text);
            }
            return S_OK;
        }

        std::string* m_output = nullptr;
    };

    struct owning_function_disassembly_data {
        std::unique_ptr<Instruction[]> m_instructions;
        std::unique_ptr<u64[]> m_symbols;
    };

    [[nodiscard]] static std::expected<ULONG, std::string> get_tlou_pid();
    [[nodiscard]] BinaryFile create_debugger_binary_file() const;

    template<typename T>
    [[nodiscard]] std::expected<T, std::string> read_virtual(u64 addr);

    [[nodiscard]] std::unique_ptr<u64[]> read_stack_frame();

    template<typename T>
    [[nodiscard]] std::expected<std::unique_ptr<T[]>, std::string> read_virtual(u64 addr, u64 num_elements);

    [[nodiscard]] std::shared_ptr<debugger_snapshot> capture_initial_snapshot(sid64 sid, ULONG thread_id);
    [[nodiscard]] res_msg<std::shared_ptr<debugger_snapshot>> capture_next_snapshot(sid64 sid, ULONG thread_id);

    void store_error(std::string_view error);

    [[nodiscard]] std::optional<COMMAND> await_command(std::stop_token st);

    WRL::ComPtr<IDebugClient> m_client;
    WRL::ComPtr<IDebugControl> m_control;
    WRL::ComPtr<IDebugRegisters> m_registers;
    WRL::ComPtr<IDebugDataSpaces4> m_data;
    WRL::ComPtr<IDebugSymbols> m_symbols;
    WRL::ComPtr<IDebugBreakpoint> m_bp;
    std::vector<owning_function_disassembly_data> m_functionData;
    std::unordered_map<sid64, std::shared_ptr<function_disassembly>> m_debuggedFunctions;
    SIDBase m_fallbackSidbase = SIDBase::from_caches({std::map<sid64, std::string>{{0, "#0"}}});
    const SIDBase* m_sidbase = &m_fallbackSidbase;
    std::map<sid64, std::string> m_sidCache;
    std::vector<sid64> m_toMatchSIDs;

    mutable std::mutex m_mutex;
    std::jthread m_debugThread;

    std::optional<std::string> m_error;
    std::shared_ptr<debugger_snapshot> m_snapshot;
    std::atomic<STATE> m_state = STATE::DETACHED;
    std::condition_variable_any m_commandCv;
    COMMAND m_command = COMMAND::NONE;

    std::optional<sid64> m_currentlyDebuggingSid = std::nullopt;

    std::string m_output;
    OutputSink m_sink;
    ULONG m_opts{};
    ULONG rax{}, rdx{}, rbp{}, rsi{}, r15{}, rdi{}, rcx{}, rsp{}, r14{};
};




}
