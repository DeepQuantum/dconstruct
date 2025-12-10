#include "base.h"

#include "llvm/BinaryFormat/DXContainer.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include <filesystem>
#include <fstream>

namespace dconstruct::shaders {

    class ndshader_file {
    public:
        explicit ndshader_file(const std::filesystem::path& path);


    private:
        llvm::dxbc::Header m_header;
        location m_dxbcStart;
        std::unique_ptr<const std::byte[]> m_bytes;
        std::unique_ptr<const llvm::dxbc::PartHeader> m_parts;
        u64 m_fsize;
    };

    ndshader_file::ndshader_file(const std::filesystem::path& path) {
        std::ifstream file(path);

        if (!file.is_open()) {
            throw std::runtime_error{"couldn't open filepath " + path.string()};
        }

        m_fsize = std::filesystem::file_size(path);
        if (m_fsize == 0) {
            throw std::runtime_error{"file is empty " + path.string()};
        }
        std::byte* temp_buffer = new std::byte[m_fsize];
        file.read(reinterpret_cast<char*>(temp_buffer), m_fsize);
        m_bytes = std::unique_ptr<std::byte[]>(temp_buffer);

        constexpr char magic[] = "ndshader";
        const char *test_chars = reinterpret_cast<const char*>(m_bytes.get());
        if (magic != test_chars) {
            throw std::runtime_error{"magic number doesn't equal 'ndshader' " + std::string(test_chars)};
        }

        constexpr u8 dxbc_offset = 0x28;
        m_dxbcStart = location(m_bytes.get() + dxbc_offset);
        m_header = m_dxbcStart.get<llvm::dxbc::Header>();
        constexpr u8 dxbc_magic[] = {'D', 'X', 'B', 'C'};
        if (m_header.Magic != dxbc_magic) {
            throw std::runtime_error{"no dxbc found"};
        }
    }

}