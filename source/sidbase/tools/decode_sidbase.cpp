#include <iostream>
#include <format>
#include <fstream>
#include <print>
#include <cstddef>
#include <cstring>
#include <memory>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::println("error: expected one argument but got {}", argc);
        return -1;
    }

    std::ifstream sidbase(argv[1], std::ios_base::binary | std::ios_base::ate);

    if (!sidbase) {
        std::println("error: couldn't open sidbase");
        return -1;
    }

    const std::size_t sidbase_size = sidbase.tellg();

    std::unique_ptr<std::byte[]> raw_bytes = std::make_unique<std::byte[]>(sidbase_size);

    sidbase.seekg(0);
    sidbase.read(reinterpret_cast<char*>(raw_bytes.get()), sidbase_size);

    std::size_t num_entries;

    std::memcpy(&num_entries, raw_bytes.get(), sizeof(num_entries));

    const std::size_t string_offset = sizeof(num_entries) + num_entries * 16;

    std::ofstream out_file("out.txt");

    const std::size_t num_chars = sidbase_size - string_offset;
    for (std::size_t i{0}; i < num_chars; ++i) {
        const char in_c = static_cast<char>(raw_bytes[string_offset + i]);
        raw_bytes[string_offset + i] = static_cast<std::byte>(in_c == '\0' ? '\n' : in_c);
    }

    out_file.write(reinterpret_cast<const char*>(raw_bytes.get()) + string_offset, num_chars);
    out_file.flush();

    return 0;
}
