#include <windows.h>
#include "MinHook.h"

constexpr uintptr_t PARSE_INSTRUCTION_OFFSET = 0x14B2D80ULL;

using fn_type = int(*)(int, int);
fn_type original_fn = nullptr;

int my_hook(int a, int b) {
    int result = original_fn(a, b);
    return result + 1;
}

void setup_hook() {
    MH_Initialize();

    uintptr_t addr = 0x12345678;
    LPVOID target = reinterpret_cast<LPVOID>(addr);

    MH_CreateHook(target, reinterpret_cast<LPVOID>(&my_hook),
                  reinterpret_cast<LPVOID*>(&original_fn));

    MH_EnableHook(target);
}