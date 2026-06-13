#include "debugger/debugger.h"
#include "base.h"

#include <DbgEng.h>
#include <atomic>
#include <cassert>
#include <cstring>
#include <disassembly/disassembler.h>
#include <expected>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string_view>


namespace dconstruct::debugger {

[[nodiscard]] resstr<ULONG> debugger::get_tlou_pid() {
    static constexpr std::wstring_view process_name = L"tlou-ii.exe"sv;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return std::unexpected{"couldn't create snapshot"};
    std::unique_ptr<void, decltype(&CloseHandle)> guard{snap, &CloseHandle};

    PROCESSENTRY32W pe{.dwSize = sizeof(pe)};
    if (!Process32FirstW(snap, &pe))
        return std::unexpected{std::format("Process32FirstW failed: {}", GetLastError())};
    do {
        if (_wcsicmp(pe.szExeFile, process_name.data()) == 0)
            return pe.th32ProcessID;
    } while (Process32NextW(snap, &pe));
    return std::unexpected{"couldn't find process name"};
}

template<typename T>
[[nodiscard]] resstr<T> debugger::read_virtual(u64 addr) {
    T value{};
    ULONG read{};
    const HRESULT hr = m_data->ReadVirtual(addr, &value, sizeof(T), &read);
    if (FAILED(hr) || read != sizeof(T)) {
        return std::unexpected{std::format("ReadVirtual failed @ {:#x} (hr={:#x}, read={}/{})", addr, static_cast<u32>(hr), read, sizeof(T))};
    }
    return value;
}

template<typename T>
[[nodiscard]] resstr<std::unique_ptr<T[]>> debugger::read_virtual(u64 addr, u64 elements) {
    std::unique_ptr<T[]> result = std::make_unique<T[]>(elements);
    ULONG read{};
    const HRESULT hr = m_data->ReadVirtual(addr, result.get(), sizeof(T) * elements, &read);
    if (FAILED(hr) || read != sizeof(T) * elements) {
        return std::unexpected{std::format("ReadVirtual failed @ {:#x} (hr={:#x}, read={}/{})", addr, static_cast<u32>(hr), read, sizeof(T) * elements)};
    }
    return result;
}

[[nodiscard]] BinaryFile debugger::create_debugger_binary_file() const {
    static constexpr u64 fake_size = sizeof(DC_Header) + 16;
    std::byte* raw_bytes = static_cast<std::byte*>(::operator new[](fake_size, std::align_val_t(64)));
    std::memset(raw_bytes, 0, fake_size);

    BinaryFile::byte_uptr bytes(raw_bytes);
    auto* header = reinterpret_cast<DC_Header*>(bytes.get());
    header->m_magic = DC_MAGIC;
    header->m_versionNumber = DC_VERSION;
    header->m_textSize = sizeof(DC_Header);
    header->m_stringsOffset = 0;
    header->field_10 = 1;
    header->m_numEntries = 0;
    header->m_pStartOfData = nullptr;

    BinaryFile file{"<debugger>", fake_size, std::move(bytes), header};

    std::byte* pointed_at = static_cast<std::byte*>(::operator new[](1, std::align_val_t(64)));
    *pointed_at = std::byte{0};
    file.m_pointedAtTable = BinaryFile::byte_uptr(pointed_at);
    file.m_relocTable = location(file.m_bytes.get() + sizeof(DC_Header) + sizeof(u32));
    file.m_strings = location(nullptr);

    return file;
}

[[nodiscard]] errmsg debugger::request_attach() {
    if (m_debugThread.joinable()) {
        return "debugger thread already running";
    }

    {
        std::lock_guard guard(m_mutex);
        m_error.reset();
        m_snapshot.reset();
    }

    m_state.store(debugger::STATE::ATTACHING, std::memory_order_release);

    m_debugThread = std::jthread([this](std::stop_token stop) {

        if (errmsg attach_error = attach()) {
            {
                std::lock_guard guard(m_mutex);
                m_error = std::move(*attach_error);
            }
            m_state.store(debugger::STATE::ERROR, std::memory_order_release);
            return;
        }

        m_state.store(debugger::STATE::ATTACHED, std::memory_order_release);
        loop(stop);
    });

    return std::nullopt;
}

void debugger::detach() {
    if (m_debugThread.joinable()) {
        m_debugThread.request_stop();
        WRL::ComPtr<IDebugControl> control;
        {
            std::lock_guard guard(m_mutex);
            control = m_control;
        }
        if (control) {
            control->SetInterrupt(DEBUG_INTERRUPT_ACTIVE);
        }
        m_debugThread.join();
    }

    cleanup_session();

    {
        std::lock_guard guard(m_mutex);
        m_snapshot.reset();
    }
    m_state.store(debugger::STATE::DETACHED, std::memory_order_release);
}

void debugger::cleanup_session() {
    if (m_bp) {
        m_bp->RemoveFlags(DEBUG_BREAKPOINT_ENABLED);
        m_bp.Reset();
    }

    if (m_symbols && m_opts != 0) {
        m_symbols->SetSymbolOptions(m_opts);
    }

    if (m_control) {
        m_control->SetExecutionStatus(DEBUG_STATUS_GO);
    }

    if (m_client) {
        m_client->SetOutputCallbacks(nullptr);
        m_client->EndSession(DEBUG_END_ACTIVE_DETACH);
        m_client->DetachProcesses();
    }

    m_symbols.Reset();
    m_data.Reset();
    m_registers.Reset();
    m_control.Reset();
    m_client.Reset();

    m_functionData.clear();
    m_debuggedFunctions.clear();
    m_currentlyDebuggingSid = std::nullopt;
    m_opts = 0;
    rax = rdx = rbp = rsi = r15 = rdi = rcx = rsp = 0;
}

[[nodiscard]] errmsg debugger::attach() {
    const resstr<ULONG> tlou_pid_res = get_tlou_pid();

    if (!tlou_pid_res) {
        return std::move(tlou_pid_res.error());
    }

    {
        std::lock_guard guard(m_mutex);
        CHECKED_THREAD(DebugCreate(__uuidof(IDebugClient), reinterpret_cast<void**>(m_client.GetAddressOf())), "couldn't create client");
        CHECKED_THREAD(m_client.As(&m_control), "couldn't create control");
        CHECKED_THREAD(m_client.As(&m_registers), "couldn't create registers");
        CHECKED_THREAD(m_client.As(&m_data), "couldn't create data");
        CHECKED_THREAD(m_client.As(&m_symbols), "couldn't create symbols");
    }

    m_client->SetOutputCallbacks(&m_sink);

    m_symbols->SetSymbolPath("");
    m_symbols->GetSymbolOptions(&m_opts);
    m_symbols->SetSymbolOptions(m_opts | SYMOPT_DEFERRED_LOADS | SYMOPT_NO_PROMPTS | SYMOPT_FAIL_CRITICAL_ERRORS);

    m_control->AddEngineOptions(DEBUG_ENGOPT_INITIAL_BREAK);

    CHECKED_THREAD(m_control->SetCodeLevel(DEBUG_LEVEL_ASSEMBLY), "couldn't set code level");

    CHECKED_THREAD(m_client->AttachProcess(0, *tlou_pid_res, DEBUG_ATTACH_DEFAULT), "couldn't attach to process");

    CHECKED_THREAD(m_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE), "initial WaitForEvent failed");

    CHECKED_THREAD(m_control->AddBreakpoint(DEBUG_BREAKPOINT_CODE, DEBUG_ANY_ID, m_bp.GetAddressOf()), "couldn't create breakpoint");

    const std::string target_name = std::format("tlou_ii+0x{:X}", PARSE_INSTRUCTION_LOOP_ENTRY);
    CHECKED_THREAD(m_bp->SetOffsetExpression(target_name.c_str()), "couldn't set bp expression");
    CHECKED_THREAD(m_bp->AddFlags(DEBUG_BREAKPOINT_ENABLED), "couldn't enable bp");

    m_registers->GetIndexByName("rax", &rax);
    m_registers->GetIndexByName("rdx", &rdx);
    m_registers->GetIndexByName("rbp", &rbp);
    m_registers->GetIndexByName("rsi", &rsi);
    m_registers->GetIndexByName("r15", &r15);
    m_registers->GetIndexByName("rdi", &rdi);
    m_registers->GetIndexByName("rcx", &rcx);
    m_registers->GetIndexByName("rsp", &rsp);
    m_registers->GetIndexByName("r14", &r14);

    return std::nullopt;
}

[[nodiscard]] errmsg debugger::poll_error() {
    std::lock_guard guard(m_mutex);
    return m_error;
}

[[nodiscard]] std::shared_ptr<debugger_snapshot> debugger::poll_snapshot() {
    if (m_state.load(std::memory_order_acquire) != STATE::SNAPSHOT_READY) {
        return nullptr;
    }
    m_state.store(debugger::STATE::ATTACHED, std::memory_order_release);
    {
        std::lock_guard guard(m_mutex);
        assert(m_snapshot && "should always have a snapshot here");
        return m_snapshot;
    }
}

void debugger::store_error(std::string_view error) {
    m_state.store(debugger::STATE::ERROR, std::memory_order_release);
    {
        std::lock_guard guard(m_mutex);
        m_error = error;
    }
}

[[nodiscard]] std::unique_ptr<u64[]> debugger::read_stack_frame() {
    DEBUG_VALUE stack_frame_start{};
    m_registers->GetValue(rdi, &stack_frame_start);

    resstr gp_registers_res = read_virtual<u64>(stack_frame_start.I64, ARGUMENT_REGISTERS_IDX * 2);

    if (!gp_registers_res) {
        store_error(gp_registers_res.error());
        return nullptr;
    }
    return std::move(*gp_registers_res);
}

void debugger::request_single_step() {
    {
        std::lock_guard guard(m_mutex);
        m_command = COMMAND::SINGLE_STEP;
    }
    m_commandCv.notify_one();
}


void debugger::request_continue() {
    {
        std::lock_guard guard(m_mutex);
        m_command = COMMAND::CONTINUE;
    }
    m_commandCv.notify_one();
}

[[nodiscard]] std::optional<debugger::COMMAND> debugger::await_command(std::stop_token st) {
    std::unique_lock lock(m_mutex);
    if (!m_commandCv.wait(lock, st, [this] { return m_command != COMMAND::NONE; })) {
        return std::nullopt;
    }
    return std::exchange(m_command, COMMAND::NONE);
}

[[nodiscard]] std::shared_ptr<debugger_snapshot> debugger::capture_initial_snapshot(const sid64 sid, const ULONG thread_id) {

    DEBUG_VALUE instruction_idx_addr{};
    m_registers->GetValue(r14, &instruction_idx_addr);

    DEBUG_VALUE script_lambda_val{};
    m_registers->GetValue(rax, &script_lambda_val);
    DEBUG_VALUE stack_frame_start{};
    m_registers->GetValue(rdi, &stack_frame_start);

    resstr<ScriptLambda> script_lamba_res = read_virtual<ScriptLambda>(script_lambda_val.I64);

    if (!script_lamba_res) {
        store_error(script_lamba_res.error());
        return nullptr;
    }

    if (script_lamba_res->m_instructionFlag != 0xdeadbeef1337f00d) {
        store_error("found function but magic number was wrong"sv);
        return nullptr;
    }

    const u64* istr_addr = script_lamba_res->m_pInstruction;
    const u64* symbol_table_addr = script_lamba_res->m_pSymbols;

    const u64 num_instructions = script_lamba_res->m_numInstructions;

    const u64 num_symbols = (script_lamba_res->m_sum - 12) / 4 - num_instructions;



    resstr instructions_res = read_virtual<Instruction>(p64(istr_addr), num_instructions);
    if (!instructions_res) {
        store_error(instructions_res.error());
        return nullptr;
    }
    resstr symbols_res = read_virtual<u64>(p64(symbol_table_addr), num_symbols);
    if (!symbols_res) {
        store_error(symbols_res.error());
        return nullptr;
    }

    std::unique_ptr stack_frame = read_stack_frame();
    if (!stack_frame) {
        return nullptr;
    }

    m_functionData.emplace_back(std::move(*instructions_res), std::move(*symbols_res));

    std::shared_ptr func_disassembly = Disassembler::create_function_disassembly(
        m_functionData.back().m_instructions.get(),
        num_instructions,
        m_sidbase->lookup(sid, m_sidCache),
        location(m_functionData.back().m_symbols.get()),
        reinterpret_cast<u64>(istr_addr),
        sizeof(Instruction),
        game_type::T2R,
        *m_sidbase,
        m_sidCache
    );

    static const char empty_debugger_string[] = "";
    for (const function_disassembly_line& line : func_disassembly->m_lines) {
        if (line.m_instruction.opcode == Opcode::LoadStaticPointerImm && line.m_instruction.operand1 < num_symbols) {
            m_functionData.back().m_symbols[line.m_instruction.operand1] = reinterpret_cast<u64>(empty_debugger_string);
        }
    }

    BinaryFile debugger_file = create_debugger_binary_file();
    auto decompiled = std::make_shared<ast::function_definition>(
        dcompiler::decomp_function{
            *func_disassembly,
            debugger_file,
            *m_sidbase,
            ControlFlowGraph::build(*func_disassembly)
        }.decompile(dcompiler::OPTIMIZATION_KIND::NONE)
    );

    resstr<u64> instruction_idx_res = read_virtual<u32>(instruction_idx_addr.I64);
    if (!instruction_idx_res) {
        store_error(std::move(instruction_idx_res.error()));
        return nullptr;
    }

    auto snapshot = std::make_unique<debugger_snapshot>();
    snapshot->m_func = std::move(func_disassembly);
    snapshot->m_decomp = std::move(decompiled);
    std::memcpy(&snapshot->m_generalPurposeRegisters, stack_frame.get(), ARGUMENT_REGISTERS_IDX * sizeof(u64));
    std::memcpy(&snapshot->m_argumentRegisters, stack_frame.get() + ARGUMENT_REGISTERS_IDX, ARGUMENT_REGISTERS_IDX * sizeof(u64));
    snapshot->m_sid = sid;
    snapshot->m_scriptLambdaPtr = script_lambda_val.I64;
    snapshot->m_stackFramePtr = stack_frame_start.I64;
    snapshot->m_debugThreadId = thread_id;
    snapshot->m_instructionIdx = *instruction_idx_res;
    return snapshot;
}

[[nodiscard]] resstr<std::shared_ptr<debugger_snapshot>> debugger::capture_next_snapshot(const sid64 sid, const ULONG thread_id) {
    DEBUG_VALUE script_lambda_val{};
    m_registers->GetValue(rax, &script_lambda_val);
    DEBUG_VALUE stack_frame_start{};
    m_registers->GetValue(rdi, &stack_frame_start);

    debugger_snapshot old_snapshot_copy{};
    {
        std::lock_guard lock(m_mutex);
        assert(m_snapshot && "next snapshot capture requires a previous snapshot");
        old_snapshot_copy = *m_snapshot;
    }

    if (old_snapshot_copy.m_sid != sid ||
        old_snapshot_copy.m_stackFramePtr != stack_frame_start.I64 ||
        old_snapshot_copy.m_debugThreadId != thread_id) {
        return nullptr;
    }

    DEBUG_VALUE instruction_idx_addr{};
    m_registers->GetValue(r14, &instruction_idx_addr);

    resstr<u64> instruction_idx_res = read_virtual<u32>(instruction_idx_addr.I64);
    if (!instruction_idx_res) {
        return std::unexpected{std::move(instruction_idx_res.error())};
    }

    const u64 instruction_idx = *instruction_idx_res;

    assert(instruction_idx == old_snapshot_copy.m_instructionIdx + 1);

    ++old_snapshot_copy.m_instructionIdx;

    std::unique_ptr stack_frame = read_stack_frame();

    if (!stack_frame) {
        return nullptr;
    }

    std::memcpy(&old_snapshot_copy.m_generalPurposeRegisters, stack_frame.get(), ARGUMENT_REGISTERS_IDX * sizeof(u64));
    std::memcpy(&old_snapshot_copy.m_argumentRegisters, stack_frame.get() + ARGUMENT_REGISTERS_IDX, ARGUMENT_REGISTERS_IDX * sizeof(u64));

    return std::make_shared<debugger_snapshot>(std::move(old_snapshot_copy));
}

void debugger::loop(std::stop_token st) {
    m_control->SetExecutionStatus(DEBUG_STATUS_GO);
    while (!st.stop_requested()) {
        if (m_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE) != S_OK) {
            break;
        }
        ULONG type{}, prod_id{}, thread_id{}, used{};
        DEBUG_LAST_EVENT_INFO_BREAKPOINT info{};
        m_control->GetLastEventInformation(&type, &prod_id, &thread_id, &info, sizeof(info), &used, nullptr, 0, nullptr);
        if (type != DEBUG_EVENT_BREAKPOINT) {
            m_control->SetExecutionStatus(DEBUG_STATUS_GO);
            continue;
        }

        DEBUG_VALUE stack_pointer{};
        m_registers->GetValue(rsp, &stack_pointer);

        resstr sid = read_virtual<sid64>(stack_pointer.I64 + SID_LOCATION_STACK_POINTER_OFFSET);
        if (!sid) {
            store_error(std::move(sid.error()));
            break;
        }

        const bool sid_found = std::any_of(m_toMatchSIDs.begin(), m_toMatchSIDs.end(), [needle = *sid] (const sid64 sid) { return needle == sid; });
        if (!sid_found) {
            m_control->SetExecutionStatus(DEBUG_STATUS_GO);
            continue;
        }

        if (m_currentlyDebuggingSid) {
            if (*m_currentlyDebuggingSid != *sid) {
                m_control->SetExecutionStatus(DEBUG_STATUS_GO);
                continue;
            }
            resstr snapshot = capture_next_snapshot(*sid, thread_id);
            if (!snapshot) {
                store_error(snapshot.error());
                break;
            } else if (*snapshot == nullptr) {
                continue;
            }
            {
                std::lock_guard lock(m_mutex);
                m_snapshot = std::move(*snapshot);
            }
        } else {
            std::shared_ptr snapshot = capture_initial_snapshot(*sid, thread_id);
            if (!snapshot) {
                break;
            }
            m_currentlyDebuggingSid = *sid;
            {
                std::lock_guard lock(m_mutex);
                m_snapshot = std::move(snapshot);
            }
        }

        m_state.store(STATE::SNAPSHOT_READY, std::memory_order_release);

        std::optional<COMMAND> command = await_command(st);
        if (!command) {
            continue;
        }
        switch (*command) {
            case COMMAND::CONTINUE: {
                m_bp->RemoveFlags(DEBUG_BREAKPOINT_ENABLED);
                m_currentlyDebuggingSid.reset();
                m_control->SetExecutionStatus(DEBUG_STATUS_GO);
                break;
            }
            case COMMAND::SINGLE_STEP: {
                m_control->SetExecutionStatus(DEBUG_STATUS_GO);
                break;
            }
            default: {
                assert(false && "unreachable, as we should only continue when we have a m_command != NONE");
            }
        }
    }
}

}
