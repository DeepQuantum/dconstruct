import struct
import sys


def fnv1a64(s):
    h = 0xCBF29CE484222325
    for c in s.encode():
        h = (0x100000001B3 * (h ^ c)) & 0xFFFFFFFFFFFFFFFF
    return h


def read_entries(path):
    with open(path, "rb") as f:
        header = f.read(32)
        magic, ver, count, data_start_lo, data_start_hi, unk32 = struct.unpack("<4sIIIII", header[:24])
        if magic != b"DSAR":
            raise ValueError(f"{path}: not DSAR (magic {magic})")
        f.seek(32)
        raw = f.read(32 * count)
        entries = []
        for i in range(count):
            e = raw[i * 32 : i * 32 + 32]
            entries.append(struct.unpack("<QQIIQ", e))
        return ver, count, entries


def main():
    path = sys.argv[1]
    names = sys.argv[2:]
    ver, count, entries = read_entries(path)
    hashes = {e[0] for e in entries}
    print(f"{path}: ver={ver:#x} count={count}")
    print("first 3 entries (h, off, comp, raw, extra):")
    for e in entries[:3]:
        print("  ", [hex(x) for x in e])
    for name in names:
        cands = {
            "asis": fnv1a64(name),
            "lower": fnv1a64(name.lower()),
            "noext": fnv1a64(name.rsplit(".", 1)[0]),
            "basename": fnv1a64(name.split("/")[-1]),
            "basenoext": fnv1a64(name.split("/")[-1].rsplit(".", 1)[0]),
        }
        hits = [k for k, h in cands.items() if h in hashes]
        print(f"  {name}: {'FOUND ' + ','.join(hits) if hits else 'not found'}")


if __name__ == "__main__":
    main()
