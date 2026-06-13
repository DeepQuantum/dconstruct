#include "base.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace dconstruct {

    std::filesystem::path executable_path() {
        std::wstring buf(MAX_PATH, L'\0');
        for (;;) {
            const DWORD len = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
            if (len == 0) {
                return {};
            }
            if (len < buf.size()) {
                buf.resize(len);
                return std::filesystem::path(std::move(buf));
            }
            buf.resize(buf.size() * 2);
        }
    }

}
