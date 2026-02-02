#include "base.h"
#include <windows.h>
#include "MinHook.h"
#include <format>
#include <print>
#include <fstream>
#include <string>
#include <iostream>
#include "disassembly/instructions.h"


namespace dconstruct::hooking {
    using QWORD = DWORD64;

    using target_fn_t = QWORD*(__fastcall*)(QWORD* a1, QWORD* a2, i64* a3);

    target_fn_t original_fn = nullptr;

    QWORD* __fastcall hook(QWORD* a1, QWORD* a2, i64* a3) {
        std::ofstream out("hook.log", std::ios::out | std::ios::app);

        const p64 instruction_idx_ptr = *(a1 + 1);
        const Instruction* instruction_ptr = *(Instruction**)instruction_idx_ptr;
        u64 i = 0;
        do {
            const Instruction& istr = instruction_ptr[i++];
            std::println(out, "{}, {}, {:x}, {:x}, {:x}" , i, istr.opcode_to_string(), istr.destination, istr.operand1, istr.operand2);
        } while(instruction_ptr[i].opcode != Opcode::Return);
        
        
        const Instruction first_instruction = instruction_ptr[0];


        // u64** v9 = (u64 **)(test + 8);

        // std::println(out, "v9: {:x}" , p64(v9));

        //const u64 op2 = (u64)*(u32*)*start_ptr;

        //out <<  "here2" << "\n";

        //const Opcode opcode = *(Opcode *)(local_2c8 + op2 * 8);

        //const std::string log_msg = std::format("{}\n",(u8)opcode);
        //out << log_msg << "\n";
        return original_fn(a1, a2, a3);
    }

    static constexpr p64 IMAGE_BASE               = 0x140000000;
    static constexpr p64 FUNCTION_RELATIVE_OFFSET = 0x1414B6940;

    static constexpr p64 FUNCTION_OFFSET          = FUNCTION_RELATIVE_OFFSET - IMAGE_BASE; 

    DWORD WINAPI init_thread(void*) {
        MH_Initialize();
        const p64 base = (p64)GetModuleHandle(nullptr);
        void* target = (void*)(base + FUNCTION_OFFSET);

        MH_CreateHook(target, &hook, reinterpret_cast<void**>(&original_fn));
        
        MH_EnableHook(target);
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