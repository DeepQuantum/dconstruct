#include "debugger/debugger.h"

#include <disassembly/disassembler.h>


namespace dconstruct::debugger {

//static constexpr ULONG64 PARSE_INSTRUCTION_BREAKPOINT_RVA = 0x1414B6B27 - CONST_MODULE_BASE;

[[nodiscard]] std::expected<ULONG, std::string> debugger::get_tlou_pid() {
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
[[nodiscard]] std::expected<T, std::string> debugger::read_virtual(u64 addr) {
    T value{};
    ULONG read{};
    const HRESULT hr = m_data->ReadVirtual(addr, &value, sizeof(T), &read);
    if (FAILED(hr) || read != sizeof(T)) {
        return std::unexpected{std::format("ReadVirtual failed @ {:#x} (hr={:#x}, read={}/{})", addr, static_cast<u32>(hr), read, sizeof(T))};
    }
    return value;
}

template<typename T>
[[nodiscard]] std::expected<std::unique_ptr<T[]>, std::string> debugger::read_virtual(u64 addr, u64 elements) {
    std::unique_ptr<T[]> result = std::make_unique<T[]>(elements);
    ULONG read{};
    const HRESULT hr = m_data->ReadVirtual(addr, result.get(), sizeof(T) * elements, &read);
    if (FAILED(hr) || read != sizeof(T) * elements) {
        return std::unexpected{std::format("ReadVirtual failed @ {:#x} (hr={:#x}, read={}/{})", addr, static_cast<u32>(hr), read, sizeof(T) * elements)};
    }
    return result;
}

[[nodiscard]] std::optional<std::string> debugger::request_attach() {
    if (m_debugThread.joinable()) {
        return "debugger thread already running";
    }

    {
        std::lock_guard lock(m_mutex);
        m_lastError.reset();
        m_attached = false;
    }

    m_debugThread = std::jthread([this](std::stop_token stop) {

        attach();

        {
            std::lock_guard lock(m_mutex);
            m_attached = !m_lastError;
        }

        if (m_lastError) {
            return;
        }

        debug_loop(stop);
    });

    return m_lastError;
}

void debugger::attach() {
    std::lock_guard g(m_mutex);
    const std::expected<ULONG, std::string> tlou_pid_res = get_tlou_pid();

    if (!tlou_pid_res) {
        m_lastError = std::move(tlou_pid_res.error());
        return;
    }
    
    CHECKED_THREAD(DebugCreate(__uuidof(IDebugClient), reinterpret_cast<void**>(m_client.GetAddressOf())), "couldn't create client");
    
    CHECKED_THREAD(m_client.As(&m_control), "couldn't create control");
    CHECKED_THREAD(m_client.As(&m_registers), "couldn't create registers");
    CHECKED_THREAD(m_client.As(&m_data), "couldn't create data");
    CHECKED_THREAD(m_client.As(&m_symbols), "couldn't create symbols");
    
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

    m_attached = true;
    m_lastError = std::nullopt;
}

template<typename sid_iter>
[[nodiscard]] std::expected<std::shared_ptr<function_disassembly>, std::string> debugger::get_sid_matched_function_disassembly(sid_iter sids_begin, sid_iter sids_end) {
    while (true) {
        m_control->SetExecutionStatus(DEBUG_STATUS_GO);
        if (m_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE) != S_OK) {
            break;
        }
        ULONG type{}, prod_id{}, thread_id{}, used{};
        DEBUG_LAST_EVENT_INFO_BREAKPOINT info{};
        m_control->GetLastEventInformation(&type, &prod_id, &thread_id, &info, sizeof(info), &used, nullptr, 0, nullptr);
        if (type != DEBUG_EVENT_BREAKPOINT) {
            continue;
        }

        DEBUG_VALUE stack_pointer{};
        
        m_registers->GetValue(rsp, &stack_pointer);

        u64 sid_location = stack_pointer.I64 + SID_LOCATION_STACK_POINTER_OFFSET;

        std::expected sid_res = read_virtual<sid64>(sid_location);

        if (!sid_res) {
            return std::unexpected{std::move(sid_res.error())};
        }

        const bool sid_match = std::any_of(sids_begin, sids_end, [found = *sid_res] (const sid64 sid_entry) { return found == sid_entry; });

        if (!sid_match) {
            continue;
        }

        DEBUG_VALUE script_lambda_val{};

        m_registers->GetValue(rax, &script_lambda_val);

        std::expected script_lamba_res = read_virtual<ScriptLambda>(script_lambda_val.I64);

        if (!script_lamba_res) {
            return std::unexpected{std::move(script_lamba_res.error())};
        }

        const u64* istr_addr = script_lamba_res->m_pInstruction;
        const u64* symbol_table_addr = script_lamba_res->m_pSymbols;

        const u64 num_instructions = script_lamba_res->m_numInstructions;

        const u64 num_symbols = (script_lamba_res->m_sum - 12) / 4 - num_instructions;
        
        std::expected instructions_res = read_virtual<Instruction>((u64)istr_addr, num_instructions);
        if (!instructions_res) {
            return std::unexpected{std::move(instructions_res.error())};
        }
        std::expected symbols_res = read_virtual<u64>((u64)symbol_table_addr, num_symbols);
        if (!symbols_res) {
            return std::unexpected{std::move(symbols_res.error())};
        }

        m_functionData.emplace_back(std::move(*instructions_res), std::move(*symbols_res));

        CHECKED(m_bp->RemoveFlags(DEBUG_BREAKPOINT_ENABLED), "couldn't disable bp");
        CHECKED(m_control->SetExecutionStatus(DEBUG_STATUS_GO), "couldn't resume");

        return Disassembler::create_function_disassembly(
            m_functionData.back().m_instructions.get(),
            num_instructions,
            "anonymous",
            location(m_functionData.back().m_symbols.get()),
            reinterpret_cast<u64>(istr_addr),
            sizeof(Instruction),
            game_type::T2R,
            m_sidbase,
            m_sidCache
        );
    }
    return std::unexpected{"debugger stopped before matching a requested SID"};
}

}