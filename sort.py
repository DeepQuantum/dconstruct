with open("./sidbase_new.bin", "rb") as f:
    _num_entries = int.from_bytes(f.read(8), "little")
    entries: dict[int, int] = {}
    for i in range(_num_entries):
        _hash = int.from_bytes(f.read(8), "little")
        _offset = int.from_bytes(f.read(8), "little")
        entries[_hash] = _offset

    new_entries = dict([(k, v) for k, v in sorted(entries.items(), key=lambda kv: kv[0])])

    for i, ((key1, value1), (key2, value2)) in enumerate(zip(entries.items(), new_entries.items())):
        if key1 != key2:
            print(f"Keys don't match at position: {i} -> {key1=} != {key2=} ({value1=} != {value2=})")

    remainder = f.read(-1)
    with open("./sidbase_new_sorted.bin", "wb") as sf:
        sf.write(_num_entries.to_bytes(8, "little"))
        for hash, offset in new_entries.items():
            sf.write(hash.to_bytes(8, byteorder="little"))
            sf.write(offset.to_bytes(8, byteorder="little"))
        sf.write(remainder)
