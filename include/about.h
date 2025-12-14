#pragma once
#include "buildinfo.h"
#include <cstdio>

static constexpr char HELP_EDIT[] = "\nusing the -e flag, you can make edits to dc files that are then put into a new copy of the original file.\n" 
    "you can specifiy the -e flag multiple times to make multiple edits at once.\n\n"
    "the syntax for an edit is as follows: \n\n    <address>[<member_offset>]=<value>.\n\n" 
    "for exmaple, say you have a structure like this:\n\n"
    "    [4] firearm-gameplay-def [0x11C28] {\n"
    "        [0] float 1.0\n"
    "        ...\n"
    "    }\n\n"
    
    "and want to replace the first member variable with the float value 0.5. the command for this would then be: \n\n"
    "    0x11C28[0]=0.5\n\n"

    "there are multiple types of member variables a struct can have: float, int, sid, string or structure. floats and ints are the easiest to replace and work like shown above (floats require a '.', ints are just numbers).\n"
    "sid replacement works in two ways: \n"
    "    0xABC[5]=ellie -> the program will try to look for the value 'ellie' in the current sidbase. if it doesn't exist, there will be a warning, and no edit will be applied.\n"
    "    0xABC[5]=#XXXXXXXXXXXXXXXX -> the program will not look for the hash and always apply the change directly. the '#' is required in this case.\n"
    "structure replacement is straightforward: you can do this to replace entire member structures with one another. say you have a struct like this:\n\n"
    "    [4] weapon-gameplay-def [0x0C523] {\n"
    "         ...\n"
    "         [7] firearm-gameplay-def [0x11C28] {\n"
    "         ...\n\n"
    "and you'd like to use a different firearm-gameplay-def for this weapon. you can then simply find the location of the firearm-gameplay-definition that you'd like to replace and put it as the replacement value: \n"
    "    0x0C523[7]=0x0ABC\n\n"
    "0x0ABC here being the address of another firearm-gameplay-def.\n"
; 

static void print_about() noexcept {
    std::printf(
        "\ndconstruct by deepquantum version %s. built with %s on %s\n\n\n" 
        "This program disassembles & decompiles (interprets the binary data) of the DC-Script files used in TLOU2.\n" 
        "you can always find tons of more info in the README.md file.\n\n"
        "SPECIAL THANKS: \n\n"
        "* icemesh for providing the underlying structures for the DC-files. (https://github.com/icemesh/dc/tree/main/t2), and his disassembler, which largely served as an inspiration for this program\n" 
        "* Specilizer for his DC-Tool, which was also used as inspiration for this program\n" 
        "* uxh for his scripting knowledge\n" 
        "* the entire modding discord for being friendly & helpful.\n\n"
        
        "All my tools/mods will always be 100%% free, but programs like this take a lot of work. If you'd like to support me, you can visit my ko-fi.\n"\
        "https://ko-fi.com/deepquantum",
        VERSION, 
        COMPILER_VERSION, 
        BUILD_DATE
    );
} 