import struct
import sys

DC_MAGIC = 0x44433030
SLOT_SIZE = 8
RELOC_BYTE_COVERAGE = 64


def align_up(value, alignment):
    return (value + alignment - 1) & ~(alignment - 1)


def main():
    if len(sys.argv) != 3:
        print(f'usage: {sys.argv[0]} <bin_file> "string"')
        return 1

    path, new_string = sys.argv[1], sys.argv[2]

    with open(path, "rb") as f:
        data = bytearray(f.read())

    if len(data) < 0x20:
        print("error: file too small to contain a DC header")
        return 1

    magic, version, text_size, strings_offset = struct.unpack_from("<4I", data, 0)
    if magic != DC_MAGIC:
        print(f"error: bad magic {magic:#x}, expected {DC_MAGIC:#x}")
        return 1
    if version != 1:
        print(f"error: unexpected version {version}")
        return 1
    if not (0x20 <= strings_offset <= text_size):
        print(f"error: strings offset {strings_offset:#x} doesn't lie within the text segment of size {text_size:#x}")
        return 1
    if text_size + 4 > len(data):
        print("error: text segment size exceeds file size")
        return 1

    (old_reloc_size,) = struct.unpack_from("<I", data, text_size)
    reloc_start = text_size + 4
    if reloc_start + old_reloc_size > len(data):
        print("error: reloc table size exceeds file size")
        return 1

    reloc = data[reloc_start:reloc_start + old_reloc_size]
    tail = data[reloc_start + old_reloc_size:]

    string_bytes = new_string.encode("utf-8") + b"\x00"
    new_string_offset = text_size
    unpadded_text_size = text_size + len(string_bytes)
    new_text_size = align_up(unpadded_text_size, 4)
    new_reloc_size = (unpadded_text_size + RELOC_BYTE_COVERAGE - 1) // RELOC_BYTE_COVERAGE

    if new_reloc_size < old_reloc_size:
        new_reloc_size = old_reloc_size
    reloc = reloc.ljust(new_reloc_size, b"\x00")

    for slot in range(strings_offset // SLOT_SIZE, len(reloc) * 8):
        if slot * SLOT_SIZE >= strings_offset and reloc[slot // 8] & (1 << (slot % 8)):
            print(f"error: string table region contains a relocated pointer at {slot * SLOT_SIZE:#x}, refusing to append")
            return 1

    out = bytearray()
    out += data[:text_size]
    out += string_bytes
    out += b"\x00" * (new_text_size - unpadded_text_size)
    out += struct.pack("<I", new_reloc_size)
    out += reloc
    out += tail

    struct.pack_into("<I", out, 0x08, new_text_size)
    struct.pack_into("<I", out, 0x0C, strings_offset)

    with open(path, "wb") as f:
        f.write(out)

    print(f"appended {len(string_bytes)} bytes at {new_string_offset:#x}")
    print(f"text size {text_size:#x} -> {new_text_size:#x}, reloc size {old_reloc_size:#x} -> {new_reloc_size:#x}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
