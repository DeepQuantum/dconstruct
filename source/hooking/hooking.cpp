#include "base.h"
#include <windows.h>
#include "MinHook.h"
#include <filesystem>
#include <format>
#include <print>
#include <fstream>
#include <mutex>
#include <string>
#include <system_error>
#include <utility>
#include "disassembly/instructions.h"


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

        std::println(out, fmt, std::forward<Args>(args)...);
    }

    QWORD* __fastcall is_final_build_hook(QWORD*);

    decltype(&is_final_build_hook) is_final_build_orig = nullptr;

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

    static constexpr p64 IMAGE_BASE                 = 0x140000000;
    static constexpr p64 RUN_SCRIPT_RELATIVE_OFFSET = 0x140D4B800;

    static constexpr p64 RUN_SCRIPT_OFFSET          = RUN_SCRIPT_RELATIVE_OFFSET - IMAGE_BASE; 

    DWORD WINAPI init_thread(void*) {
        MH_Initialize();
        const p64 base = (p64)GetModuleHandle(nullptr);
        void* is_final_build_target = (void*)(base + RUN_SCRIPT_OFFSET);
        MH_CreateHook(is_final_build_target, &is_final_build_hook, reinterpret_cast<void**>(&is_final_build_orig));
        MH_EnableHook(is_final_build_target);
        return 0;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, dconstruct::hooking::init_thread, nullptr, 0, nullptr);
    }
    return TRUE;
}
