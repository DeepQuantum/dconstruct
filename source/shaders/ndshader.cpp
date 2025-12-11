#include "shaders/ndshader.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Object/DXContainer.h"
#include "llvm/BinaryFormat/DXContainer.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Support/DXILABI.h"
#include "llvm/Transforms/Utils/DXILUpgrade.h"
#include "llvm/Support/Error.h"
#include <filesystem>
#include <fstream>

namespace dconstruct::shaders {

[[nodiscard]] ndshader_file::expected ndshader_file::parse_from_file(const std::filesystem::path& path) noexcept {
    std::ifstream file(path);
    if (!file.is_open()) {
        return std::unexpected{"couldn't open filepath " + path.string()};
    }
    const u64 fsize = std::filesystem::file_size(path);
    if (fsize == 0) {
        return std::unexpected{"file is empty " + path.string()};
    }
    std::byte* temp_buffer = new std::byte[fsize];
    file.read(reinterpret_cast<char*>(temp_buffer), fsize);
    auto bytes = std::unique_ptr<const std::byte[]>(temp_buffer);
    constexpr std::array magic = {'n', 'd', 's', 'h', 'a', 'd', 'e', 'r'};
    if (std::memcmp(&magic, bytes.get(), magic.size())) {
        return std::unexpected{"magic number doesn't equal 'ndshader' " + std::string(reinterpret_cast<const char*>(bytes.get()))};
    }

    auto buffer = llvm::MemoryBuffer::getFileSlice(path.string(), fsize - NDSHADER_DXBC_OFFSET, NDSHADER_DXBC_OFFSET);
    if (!buffer) {
        return std::unexpected{"couldn't get file slice"};
    }
    auto container = llvm::object::DXContainer::create(buffer.get()->getMemBufferRef());
    if (auto e = container.takeError()) {
        return std::unexpected{llvm::toString(std::move(e))};
    }
    return ndshader_file(path, std::move(*container));
}

[[nodiscard]] std::string ndshader_file::to_string() const noexcept {
    std::stringstream out;

    out << "=== ndshader file === \n";
    out << "header: \n";

    const auto& header = m_dxcontainer.getHeader();

    out << " magic: DXBC\n";
    out << " digest: ";
    out << std::hex;
    for (u8 i = 0; i < sizeof(header.FileHash.Digest); ++i) {
        out << static_cast<u16>(header.FileHash.Digest[i]);
    }
    out << "\n" << std::dec;
    out << " major version: " << header.Version.Major << "\n";
    out << " minor version: " << header.Version.Minor << "\n";
    out << " file size: " << header.FileSize << " bytes\n";
    out << " part count: " << header.PartCount << "\n";

    out << "=== DXBC parts ===\n\n";

    for (const auto& part : m_dxcontainer) {
        const auto& name = part.Part.getName().str();
        out << "=== " << name << " ===\n\n";

        if (name == "DXIL") {
            llvm::StringRef program_header = part.Data;
            const auto* program_header_ptr = reinterpret_cast<const llvm::dxbc::ProgramHeader*>(program_header.data());
            const auto* bitcode_header = &program_header_ptr->Bitcode;
            const auto bitcode = llvm::StringRef(reinterpret_cast<const char*>(bitcode_header) + bitcode_header->Offset, bitcode_header->Size);

            auto buffer = llvm::MemoryBuffer::getMemBuffer(bitcode);

            auto ctx = std::make_unique<llvm::LLVMContext>();

            auto mod = llvm::parseBitcodeFile(buffer->getMemBufferRef(), *ctx);

            std::string ir;
            llvm::raw_string_ostream os(ir);

            mod->get()->print(os, nullptr);
            os.flush();
            out << ir;
        }
    }

    return out.str();
}
}