import hashlib
import struct
import sys


def read_toc(path):
    with open(path, "rb") as f:
        header = f.read(32)
        magic, ver, comp, toc_len, entry_size, count, block_size, flags = struct.unpack(
            ">4sI4sIIIII", header
        )
        if magic != b"PSAR":
            raise ValueError(f"{path}: not a psarc")
        f.seek(20 if False else 32)
        f.seek(32)
        toc = f.read(entry_size * count)
        hashes = set()
        for i in range(count):
            entry = toc[i * entry_size : i * entry_size + entry_size]
            hashes.add(entry[:16])
        return {
            "version": ver,
            "comp": comp,
            "entry_size": entry_size,
            "count": count,
            "flags": flags,
            "hashes": hashes,
        }


def variants(name):
    out = {}
    for prefix in ("", "/"):
        for case in ("asis", "upper", "lower"):
            p = prefix + name
            if case == "upper":
                p = p.upper()
            elif case == "lower":
                p = p.lower()
            out[f"{prefix or 'noslash'}|{case}"] = hashlib.md5(p.encode()).digest()
    return out


def probe(archive, names):
    toc = read_toc(archive)
    print(
        f"{archive}: version={toc['version']:#x} comp={toc['comp']} entries={toc['count']} flags={toc['flags']:#x}"
    )
    for name in names:
        hits = [k for k, h in variants(name).items() if h in toc["hashes"]]
        print(f"  {name}: {'FOUND ' + ','.join(hits) if hits else 'not found'}")


if __name__ == "__main__":
    archive = sys.argv[1]
    names = sys.argv[2:]
    probe(archive, names)
