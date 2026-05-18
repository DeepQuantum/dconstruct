#include "base.h"
#include "sidbase.h"
#include <windows.h>
#include "MinHook.h"
#include <filesystem>
#include <format>
#include <print>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <utility>


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
        out << std::vformat(fmt, std::make_format_args(args...)) << '\n';
    }

    QWORD* __fastcall is_final_build_hook(QWORD*);

    QWORD* __fastcall display_hook(QWORD*, QWORD*, const char**);

    char __fastcall invoke_function_hook(i64 a1, i64 a2, u32 a3, QWORD *a4, i64 a5);

    QWORD* __fastcall lookup_hook(u64 a1);

    decltype(&is_final_build_hook) is_final_build_orig = nullptr;
    decltype(&display_hook) display_orig = nullptr;
    decltype(&invoke_function_hook) invoke_function_orig = nullptr;
    decltype(&lookup_hook) lookup_orig = nullptr;

    static constexpr p64 IMAGE_BASE                 = 0x140000000;

    p64 module_base = 0;
    p64 module_end = 0;
    std::unique_ptr<SIDBase> sidbase;

    void load_sidbase() {
        const std::filesystem::path sidbase_path = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dc_test_files/sidbase_new_fixed.bin";
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
            reinterpret_cast<const std::byte*>(module) + dos_header->e_lfanew
        );
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
    

    // QWORD* __fastcall hook(QWORD* a1, QWORD* a2, i64* a3) {
    //     std::ofstream out("hook.log", std::ios::out | std::ios::app);

    //     const p64 instruction_idx_ptr = *(a1 + 1);
    //     const Instruction* instruction_ptr = *(Instruction**)instruction_idx_ptr;
    //     u64 i = 0;
    //     do {
    //         const Instruction& istr = instruction_ptr[i++];
    //         std::println(out, "{}, {}, {:x}, {:x}, {:x}" , i, istr.opcode_to_string(), istr.destination, istr.operand1, istr.operand2);
    //     } while(instruction_ptr[i].opcode != Opcode::Return);
        
        
    //     const Instruction first_instruction = instruction_ptr[0];


    //     // u64** v9 = (u64 **)(test + 8);

    //     // std::println(out, "v9: {:x}" , p64(v9));

    //     //const u64 op2 = (u64)*(u32*)*start_ptr;

    //     //out <<  "here2" << "\n";

    //     //const Opcode opcode = *(Opcode *)(local_2c8 + op2 * 8);

    //     //const std::string log_msg = std::format("{}\n",(u8)opcode);
    //     //out << log_msg << "\n";
    //     return original_fn(a1, a2, a3);
    // }

    QWORD* __fastcall is_final_build_hook(QWORD* a1) {
       // log("called is_final_build?");
       // char orig_res = is_final_build_orig(a1, a2, a3, a4, a5);
        *a1 = false;
        return a1;
    }

    QWORD* __fastcall display_hook(QWORD* a1, QWORD* __dummy, const char** message) {
        log_runtime(*message);
        *a1 = 0;
        return a1;
    }

    // stack size: 0x40
    // a5 location: rbp + 0x28
    // at breakpoint, SID is located at rsp + 0x80
    // at hook, rsp will be 0x40 smaller -> SID will be at rsp + 0x40
    // if a5 = rsp + 0xA0 and 
    char __fastcall invoke_function_hook(i64 a1, i64 a2, u32 a3, QWORD *a4, i64 a5) {
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
    

    static constexpr p64 IS_FINAL_BUILD_OFFSET      = 0x140D4B800 - IMAGE_BASE;
    static constexpr p64 DISPLAY_OFFSET             = 0x140D3DA40 - IMAGE_BASE;
    static constexpr p64 INVOKE_FUNCTION_OFFSET     = 0x141613510 - IMAGE_BASE;
    static constexpr p64 LOOKUP_OFFSET              = 0x141356D60 - IMAGE_BASE;

    DWORD WINAPI init_thread(void*) {
        MH_Initialize();
        HMODULE executable_module = GetModuleHandle(nullptr);
        module_base = reinterpret_cast<p64>(executable_module);
        module_end = image_end(executable_module);
        load_sidbase();
        log("module base: {:016X}, ida image base: {:016X}", module_base, IMAGE_BASE);

        // void* display_target = (void*)(base + DISPLAY_OFFSET);
        // MH_CreateHook(display_target, &display_hook, reinterpret_cast<void**>(&display_orig));
        // MH_EnableHook(display_target);

        // void* invoke_function_target = (void*)(base + INVOKE_FUNCTION_OFFSET);
        // MH_CreateHook(invoke_function_target, &invoke_function_hook, reinterpret_cast<void**>(&invoke_function_orig));
        // MH_EnableHook(invoke_function_target);

        void* lookup_target = (void*)(module_base + LOOKUP_OFFSET);
        MH_CreateHook(lookup_target, &lookup_hook, reinterpret_cast<void**>(&lookup_orig));
        MH_EnableHook(lookup_target);
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
