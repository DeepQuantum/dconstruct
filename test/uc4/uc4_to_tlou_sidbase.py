def main():
    with open("sidbase.bin", "rb") as f:
        _num_entries = int.from_bytes(f.read(4), "big")
        entries: dict[int, int] = {}
        string_start = _num_entries * 16 + 8
        for i in range(_num_entries):
            _hash = int.from_bytes(f.read(4), "big")
            _offset = int.from_bytes(f.read(4), "big")
            entries[_hash] = _offset + string_start

        new_entries = dict([(k, v) for k, v in sorted(entries.items(), key=lambda kv: kv[0])])

        remainder = f.read(-1)
    with open("./sidbase_sorted.bin", "wb") as sf:
        sf.write(_num_entries.to_bytes(8, "little"))
        for hash, offset in new_entries.items():
            sf.write(hash.to_bytes(8, byteorder="little"))
            sf.write(offset.to_bytes(8, byteorder="little"))
        sf.write(remainder)


if __name__ == "__main__":
    main()
