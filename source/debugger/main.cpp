#include <windows.h>
#include <DbgEng.h>
#include <wrl/client.h>
#include <print>
#include <format>
#include <cstdlib>
#include <string_view>
#include <dbghelp.h>
#include <tlhelp32.h>

#include "base.h"
#include <expected>

#define CHECKED(expr, message) if (FAILED(expr)) { std::println(stderr, message); return -1; }



using namespace std::literals;
using namespace Microsoft;


namespace dconstruct::debugger {


static constexpr ULONG64 CONST_MODULE_BASE = 0x140000000;
static constexpr ULONG64 PARSE_INSTRUCTION_BREAKPOINT_RVA = 0x1414B6B27 - CONST_MODULE_BASE;


static std::expected<ULONG, std::string> get_tlou_pid() {
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

bool enable_debug_privilege() {
    HANDLE token{};

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return false;
    }

    LUID luid{};

    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid)) {
        CloseHandle(token);
        return false;
    }

    TOKEN_PRIVILEGES privileges{};
    privileges.PrivilegeCount = 1;
    privileges.Privileges[0].Luid = luid;
    privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    const BOOL ok = AdjustTokenPrivileges(token, FALSE, &privileges, sizeof(privileges), nullptr, nullptr);
    const DWORD err = GetLastError();

    CloseHandle(token);

    return ok && err == ERROR_SUCCESS;
}

struct OutputSink : IDebugOutputCallbacks {
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
        std::fputs(text, stderr);
        return S_OK;
    }
};

i32 run(void) {
    const std::expected<ULONG, std::string> tlou_pid = get_tlou_pid();

    std::println(stderr, "target pid = {}", *tlou_pid);

    if (!tlou_pid) {
        std::println(stderr, "cannot attach dbgeng to the target process");
        return -1;
    }
    
    WRL::ComPtr<IDebugClient> client;
    CHECKED(DebugCreate(__uuidof(IDebugClient), reinterpret_cast<void**>(client.GetAddressOf())), "couldn't create client");
    WRL::ComPtr<IDebugControl> control;
    WRL::ComPtr<IDebugRegisters> registers;
    WRL::ComPtr<IDebugDataSpaces4> data;
    WRL::ComPtr<IDebugSymbols> symbols;
    CHECKED(client.As(&control), "couldn't create control");
    CHECKED(client.As(&registers), "couldn't create registers");
    CHECKED(client.As(&data), "couldn't create data");
    CHECKED(client.As(&symbols), "couldn't create symbols");
    
    static OutputSink sink;
    client->SetOutputCallbacks(&sink);

    symbols->SetSymbolPath("");
    ULONG opts{};
    symbols->GetSymbolOptions(&opts);
    symbols->SetSymbolOptions(opts | SYMOPT_DEFERRED_LOADS | SYMOPT_NO_PROMPTS | SYMOPT_FAIL_CRITICAL_ERRORS);

    control->AddEngineOptions(DEBUG_ENGOPT_INITIAL_BREAK);

    CHECKED(client->AttachProcess(0, *tlou_pid, DEBUG_ATTACH_DEFAULT), "couldn't attach to process");

    CHECKED(control->SetInterrupt(DEBUG_INTERRUPT_ACTIVE), "couldn't set interrupt")
    CHECKED(control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE), "initial WaitForEvent failed");

    WRL::ComPtr<IDebugBreakpoint> bp;
    CHECKED(control->AddBreakpoint(DEBUG_BREAKPOINT_CODE, DEBUG_ANY_ID, bp.GetAddressOf()), "couldn't create breakpoint");
    
    const std::string target_name = std::format("tlou_ii+0x{:X}", PARSE_INSTRUCTION_BREAKPOINT_RVA);
    CHECKED(bp->SetOffsetExpression(target_name.c_str()), "couldn't set bp expression");
    CHECKED(bp->AddFlags(DEBUG_BREAKPOINT_ENABLED), "couldn't enable bp");

    ULONG rax_idx{};
    registers->GetIndexByName("rax", &rax_idx);
    while (true) {
        control->SetExecutionStatus(DEBUG_STATUS_GO);
        if (control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE) != S_OK) {
            break;
        }
        ULONG type{}, prod_id{}, thread_id{}, used{};
        DEBUG_LAST_EVENT_INFO_BREAKPOINT info{};
        control->GetLastEventInformation(&type, &prod_id, &thread_id, &info, sizeof(info), &used, nullptr, 0, nullptr);
        if (type != DEBUG_EVENT_BREAKPOINT) {
            continue;
        }
        DEBUG_VALUE val{};
        registers->GetValue(rax_idx, &val);
    }
    client->DetachProcesses();
    return 0;
}

};

int main(void) {
    return dconstruct::debugger::run();
}
