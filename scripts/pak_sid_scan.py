import mmap
import os
import struct
import sys


def fnv1a64(s):
    h = 0xCBF29CE484222325
    for c in s.encode():
        h = (0x100000001B3 * (h ^ c)) & 0xFFFFFFFFFFFFFFFF
    return h


def scan_file(path, needles):
    found = set()
    size = os.path.getsize(path)
    if size == 0:
        return found
    with open(path, "rb") as f:
        with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as m:
            for name, h in needles.items():
                pat = struct.pack("<Q", h)
                if m.find(pat) != -1:
                    found.add(name)
    return found


def main():
    names_file = sys.argv[1]
    targets = sys.argv[2:]
    with open(names_file) as f:
        names = [line.strip() for line in f if line.strip()]
    needles = {n: fnv1a64(n) for n in names}
    for n, h in needles.items():
        print(f"sid {h:016x} = {n}")
    files = []
    for t in targets:
        if os.path.isdir(t):
            for root, _, fs in os.walk(t):
                files.extend(os.path.join(root, x) for x in fs)
        else:
            files.append(t)
    for path in files:
        hits = scan_file(path, needles)
        if hits:
            print(f"{os.path.basename(path)}: {', '.join(sorted(hits))}")


if __name__ == "__main__":
    main()
