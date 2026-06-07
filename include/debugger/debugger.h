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
#include "sidbase.h"

#include "DCScript.h"

#define CHECKED_THREAD(expr, message) if (FAILED(expr)) { m_lastError = message; return; }

using namespace std::literals;
using namespace Microsoft;

namespace dconstruct::debugger {

    
static constexpr ULONG64 PARSE_INSTRUCTION_LOOP_ENTRY = 0x0014B6B10;
static constexpr ULONG64 SID_LOCATION_STACK_POINTER_OFFSET = 0x338;
 
class debugger {

public:
    [[nodiscard]] std::optional<std::string> request_attach();

    debugger() {
        m_sink.set_output(&m_output);
    }

    [[nodiscard]] bool is_attached() const noexcept {
        return m_attached;
    }
    [[nodiscard]] const std::string& output() const noexcept {
        return m_output;
    }
    void append_output(std::string_view text) {
        m_output.append(text);
    }

    template<typename sid_iter>
    [[nodiscard]] std::expected<std::shared_ptr<function_disassembly>, std::string> get_sid_matched_function_disassembly(sid_iter begin, sid_iter end);

    debugger(debugger&) = delete;
    debugger(debugger&&) = delete;

    debugger& operator=(debugger&) = delete;
    debugger& operator=(debugger&&) = delete;

    ~debugger() {
        if (m_client) {
            m_client->DetachProcesses();
        }
    }

private:
    void attach();

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
    
    template<typename T>
    [[nodiscard]] std::expected<T, std::string> read_virtual(u64 addr);

    template<typename T>
    [[nodiscard]] std::expected<std::unique_ptr<T[]>, std::string> read_virtual(u64 addr, u64 num_elements);

    WRL::ComPtr<IDebugClient> m_client;
    WRL::ComPtr<IDebugControl> m_control;
    WRL::ComPtr<IDebugRegisters> m_registers;
    WRL::ComPtr<IDebugDataSpaces4> m_data;
    WRL::ComPtr<IDebugSymbols> m_symbols;
    WRL::ComPtr<IDebugBreakpoint> m_bp;
    std::vector<owning_function_disassembly_data> m_functionData;
    std::unordered_map<sid64, std::shared_ptr<function_disassembly>> m_debuggedFunctions;
    SIDBase m_sidbase = SIDBase::from_caches({std::map<sid64, std::string>{{0, "#0"}}});
    std::map<sid64, std::string> m_sidCache;

    std::jthread m_debugThread;
    std::mutex m_mutex;
    std::optional<std::string> m_lastError;

    std::string m_output;
    OutputSink m_sink;
    bool m_attached = false;
    ULONG m_opts{};
    ULONG rax{}, rdx{}, rbp{}, rsi{}, r15{}, rdi{}, rcx{}, rsp{};
};
    



}
