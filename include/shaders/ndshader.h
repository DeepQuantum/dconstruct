#pragma once

#include "../base.h"

#include "llvm/Object/DXContainer.h"

#include <expected>
#include <filesystem>

namespace dconstruct::shaders {

    // struct ndshader {
    //     char magic[8];
    //     u32 always_3;
    // };

    class ndshader_file {
        static constexpr u8 NDSHADER_DXBC_OFFSET = 0x28;

    public:
        using expected = std::expected<ndshader_file, std::string>;

        [[nodiscard]] static ndshader_file::expected parse_from_file(const std::filesystem::path &path) noexcept;

        [[nodiscard]] std::tuple<std::string, bool> to_string() const noexcept;
    
    private:
        std::filesystem::path m_filepath;
        std::unique_ptr<llvm::MemoryBuffer> m_dxbcbuffer;
        std::string m_IR;

        explicit ndshader_file(std::filesystem::path path, std::unique_ptr<llvm::MemoryBuffer>&& buffer) : m_filepath(std::move(path)), m_dxbcbuffer(std::move(buffer)) {};
    };
}
