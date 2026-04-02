def main() -> None:
    with open("sidbase_new.bin", "rb") as f:
        new_bytes = f.read()
    unknown_3 = int.from_bytes(new_bytes[:8], "little")
    num_entries = int.from_bytes(new_bytes[8:16], "little")
    string_offset = 32 + (num_entries - 1) * 16
    out_bytes: bytearray = bytearray(num_entries.to_bytes(8, "little"))

    for i in range(num_entries - 1):
        base = i * 16 + 32
        _hash = int.from_bytes(new_bytes[base : base + 8], "little")
        _offset = int.from_bytes(new_bytes[base + 8 : base + 16], "little")
        new_offset = string_offset + _offset - 24
        out_bytes += _hash.to_bytes(8, "little")
        out_bytes += new_offset.to_bytes(8, "little")

    out_bytes += new_bytes[string_offset:]

    with open("sidbase_new_fixed.bin", "wb") as f:
        f.write(out_bytes)


if __name__ == "__main__":
    main()
