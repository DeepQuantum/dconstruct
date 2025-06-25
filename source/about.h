#pragma once

#include "buildinfo.h"
#include <cstdio>

static void print_about() noexcept {
    std::printf(
        "\ntlouii dc-disassembler version %s. built with %s on %s\n\n\n" \
        "This program disassembles (interprets the binary data) of the DC-Script files used in TLOU2.\n" \
        "It outputs large text files containing structured data of the orinal DC-file. It's optimized for data accuracy and speed.\n" \
        "It also allows you to make edits to files via the command line, including replacing entire structures with little effort. use tlou2_disasm --help edit for more info.\n"\
        "Use --help to find all the options available. This is a beta version, so there might be bugs, incorrect data or" \
        " unexpected crashes. If you encounter any of these, please let me know, and include your operating system plus the command.\n" \
        " you were running. There will be many more features in future updates.\n\n"\
        "SPECIAL THANKS: \n\n"\
        "* icemesh for providing the underlying structures for the DC-files. (https://github.com/icemesh/dc/tree/main/t2), and his disassembler, which largely served as an inspiration for this program\n" \
        "* Specilizer for his DC-Tool, which was also used as inspiration for this program\n" \
        "* uxh for his scripting knowledge\n" \
        "* the entire modding discord for being friendly & helpful.\n\n"
        
        "All my tools/mods will always be 100%% free, but programs like this take a lot of work. If you'd like to support me, you can visit my ko-fi.\n"\
        "https://ko-fi.com/deepquantum",
        "beta_1", 
        COMPILER_VERSION, 
        BUILD_DATE
    );
} 