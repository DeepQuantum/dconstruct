#include "base.h"


namespace dconstruct {

    class Emitter {
    public:
        template <typename ...Args>
        void operator()(const char *format, Args ...args) noexcept {
            char buffer[m_defaultBufferSize];
            snprintf(buffer, m_defaultBufferSize, format, args);
            m_buffer += buffer;
        }
    
    private:
        std::string m_buffer;
        constexpr static const u32 m_defaultBufferSize = 1024;
    };
}