#include "base.h"
#include "extended_opcodes.h"
#include "sidbase.h"
#include <windows.h>
#include "MinHook.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <mutex>
#include <utility>
#include <array>

namespace dconstruct::hooking {
    using QWORD = DWORD64;

    template <typename... Args>
    void log(std::format_string<Args...> fmt, Args&&... args) {
        static std::mutex log_mutex;
        std::scoped_lock lock(log_mutex);

        std::ofstream out("C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/dconstruct_log.txt", std::ios::out | std::ios::app);
        if (!out) {
            return;
        }

        out << std::format(fmt, std::forward<Args>(args)...) << '\n';
    }

    template <typename... Args>
    void log_runtime(std::string_view fmt, Args&&... args) {
        static std::mutex log_mutex;
        std::scoped_lock lock(log_mutex);
        std::ofstream out("C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/dconstruct_log.txt", std::ios::out | std::ios::app);
        if (!out) {
            return;
        }
        const auto now = std::chrono::current_zone()->to_local(
            std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));
        out << std::format("[{:%H:%M:%S}] ", now) << std::vformat(fmt, std::make_format_args(args...)) << '\n';
    }

    QWORD* __fastcall is_final_build_hook(QWORD*);

    QWORD* __fastcall display_hook(QWORD*, QWORD*, const char**);

    char __fastcall invoke_function_hook(i64, i64, u32, QWORD*, i64);

    QWORD* __fastcall lookup_hook(u64 a1);

    QWORD* __fastcall debug_draw_string_2d_hook(QWORD*, i32, i64, i64);

    decltype(&is_final_build_hook) is_final_build_orig = nullptr;
    decltype(&display_hook) display_orig = nullptr;
    decltype(&invoke_function_hook) invoke_function_orig = nullptr;
    decltype(&lookup_hook) lookup_orig = nullptr;

    static constexpr p64 IMAGE_BASE = 0x140000000;

    p64 module_base = 0;
    p64 module_end = 0;
    std::unique_ptr<SIDBase> sidbase;

    void load_sidbase() {
        const std::filesystem::path sidbase_path = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/fixtures/dc/sidbase_new_fixed.bin";
        auto sidbase_res = SIDBase::from_binary(sidbase_path);
        if (!sidbase_res) {
            log("failed to load sidbase '{}': {}", sidbase_path.string(), sidbase_res.error());
            return;
        }

        sidbase = std::make_unique<SIDBase>(std::move(*sidbase_res));
        log("loaded sidbase '{}'", sidbase_path.string());
    }

    [[nodiscard]] const char* resolve_sid(sid64 sid) {
        if (sidbase == nullptr) {
            return nullptr;
        }
        return sidbase->search(sid);
    }

    [[nodiscard]] p64 image_end(HMODULE module) {
        if (module == nullptr) {
            return 0;
        }

        const auto* dos_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
        if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
            return 0;
        }

        const auto* nt_headers = reinterpret_cast<const IMAGE_NT_HEADERS*>(
            reinterpret_cast<const std::byte*>(module) + dos_header->e_lfanew);
        if (nt_headers->Signature != IMAGE_NT_SIGNATURE) {
            return 0;
        }

        return reinterpret_cast<p64>(module) + nt_headers->OptionalHeader.SizeOfImage;
    }

    [[nodiscard]] bool try_ida_address(p64 address, p64& out) {
        if (module_base == 0 || module_end <= module_base || address < module_base || address >= module_end) {
            return false;
        }

        out = IMAGE_BASE + (address - module_base);
        return true;
    }

    QWORD* __fastcall is_final_build_hook(QWORD* a1) {
        // log("called is_final_build?");
        // char orig_res = is_final_build_orig(a1, a2, a3, a4, a5);
        *a1 = false;
        return a1;
    }

    QWORD* __fastcall display_hook(QWORD* a1, QWORD* __dummy, const char** message) {
        if (message) {
            log_runtime(*message);
        }
        *a1 = 0;
        return a1;
    }

    char __fastcall invoke_function_hook(i64 a1, i64 a2, u32 a3, QWORD* a4, i64 a5) {
        p64 function_address = 0;
        if (!try_ida_address(*(p64*)(a1 + 0x8), function_address)) {
            return invoke_function_orig(a1, a2, a3, a4, a5);
        }

        for (u64 i = 0; i < 0x160; i += 8) {
            const sid64 sid = *(sid64*)((char*)&a5 + i);
            const char* resolved_sid = resolve_sid(sid);
            if (resolved_sid != nullptr) {
                log("hooked invoke function with i: {:01} sid: {:016X} ({}) at address: {:016X}", i, sid, resolved_sid, function_address);
            }
        }

        return invoke_function_orig(a1, a2, a3, a4, a5);
    }

    QWORD* __fastcall lookup_hook(u64 a1) {
        QWORD* res = lookup_orig(a1);

        if (res != nullptr) {
            const char* resolved_sid = resolve_sid(a1);
            if (resolved_sid == nullptr) {
                return res;
            }

            p64 func_ptr = 0;
            if (try_ida_address(*(*(QWORD**)res + 1), func_ptr)) {
                log("{:016X} ({}) func_ptr: {:016X}", a1, resolved_sid, func_ptr);
            }
        }

        return res;
    }

    static constexpr p64 STORE_ARRAY_RUN_SCRIPT = 0x1413566EE - IMAGE_BASE;
    static constexpr u64 NOP_LENGTH_RUN_SCRIPT = 0x141356709 - 0x1413566EE;

    static constexpr p64 STORE_ARRAY_PARSE_INSTRUCTION = 0x1414B7084 - IMAGE_BASE;
    static constexpr u64 NOP_LENGTH_PARSE_INSTRUCTION = 0x1414B70A1 - 0x1414B7084;

    static constexpr p64 DEFAULT_CASE_RUN_SCRIPT = 0x141356969 - IMAGE_BASE;

    static constexpr p64 CONTINUE_ADDRESS = 0x1413560A0 - IMAGE_BASE;

    static constexpr p64 IS_FINAL_BUILD_OFFSET = 0x140D4B800 - IMAGE_BASE;
    static constexpr p64 DISPLAY_OFFSET = 0x140D3DA40 - IMAGE_BASE;
    static constexpr p64 INVOKE_FUNCTION_OFFSET = 0x141613510 - IMAGE_BASE;
    static constexpr p64 LOOKUP_OFFSET = 0x141356D60 - IMAGE_BASE;
    static constexpr p64 DEBUG_DRAW_SRING_2D_OFFSET = 0x140D3BE10 - IMAGE_BASE;

    void create_opcode_default_switch_override(p64 start_default_block, p64 target_location, p64 continue_location) {
        log("target_location: {:x}, start_default_block: {:x}", target_location, start_default_block);

        constexpr std::array patch_bytes = {
            0xFF_b,
            0x35_b,
            0x06_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0xFF_b,
            0x25_b,
            0x08_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
            0x00_b,
        };

        DWORD old_protect;
        VirtualProtect((void*)start_default_block, patch_bytes.size(), PAGE_EXECUTE_READWRITE, &old_protect);
        std::memcpy((void*)start_default_block, patch_bytes.data(), patch_bytes.size());
        std::memcpy((void*)(start_default_block + 12), &continue_location, sizeof(p64));
        std::memcpy((void*)(start_default_block + 20), &target_location, sizeof(p64));
        VirtualProtect((void*)start_default_block, patch_bytes.size(), old_protect, &old_protect);
        FlushInstructionCache(GetCurrentProcess(), (void*)start_default_block, patch_bytes.size());
    }

    void create_breakpoint_entry(p64 start_loc, u64 length) {
        std::vector<std::byte> nops{length, 0x90_b};
        nops.back() = 0xCC_b;

        DWORD old_protect;
        VirtualProtect((void*)start_loc, length, PAGE_EXECUTE_READWRITE, &old_protect);
        std::memcpy((void*)start_loc, nops.data(), length);
        VirtualProtect((void*)start_loc, length, old_protect, &old_protect);
        FlushInstructionCache(GetCurrentProcess(), (void*)start_loc, length);
        log("created breakpoint entry at {:X}", start_loc);
    }

    DWORD WINAPI init_thread(void*) {
        MH_Initialize();
        HMODULE executable_module = GetModuleHandle(nullptr);
        module_base = reinterpret_cast<p64>(executable_module);
        module_end = image_end(executable_module);

        create_breakpoint_entry(module_base + STORE_ARRAY_RUN_SCRIPT, NOP_LENGTH_RUN_SCRIPT);
        create_breakpoint_entry(module_base + STORE_ARRAY_PARSE_INSTRUCTION, NOP_LENGTH_PARSE_INSTRUCTION);

        const p64 jump_extended_opcode_switch_location = (p64)&extended_opcode_switch;
        const p64 extended_opcode_switch_location = (jump_extended_opcode_switch_location + 5) + (*(i32*)(jump_extended_opcode_switch_location + 1));
        create_opcode_default_switch_override(module_base + DEFAULT_CASE_RUN_SCRIPT, extended_opcode_switch_location, module_base + CONTINUE_ADDRESS);

        LPVOID display_target = reinterpret_cast<LPVOID>(module_base + DISPLAY_OFFSET);
        MH_CreateHook(display_target, reinterpret_cast<LPVOID>(&display_hook), reinterpret_cast<LPVOID*>(&display_orig));
        MH_EnableHook(display_target);

        // LPVOID is_final_build_target = reinterpret_cast<LPVOID>(module_base + IS_FINAL_BUILD_OFFSET);
        // MH_CreateHook(is_final_build_target, reinterpret_cast<LPVOID>(&is_final_build_hook), reinterpret_cast<LPVOID*>(&is_final_build_orig));
        // MH_EnableHook(is_final_build_target);

        // void* invoke_function_target = (void*)(base + INVOKE_FUNCTION_OFFSET);
        // MH_CreateHook(invoke_function_target, &invoke_function_hook, reinterpret_cast<void**>(&invoke_function_orig));
        // MH_EnableHook(invoke_function_target);

        // void* lookup_target = (void*)(module_base + LOOKUP_OFFSET);
        // MH_CreateHook(lookup_target, &lookup_hook, reinterpret_cast<void**>(&lookup_orig));
        // MH_EnableHook(lookup_target);
        return 0;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, dconstruct::hooking::init_thread, hModule, 0, nullptr);
    }
    return TRUE;
}
