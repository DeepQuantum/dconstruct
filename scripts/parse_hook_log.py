import re
import sys


LOG_RE = re.compile(
    r"(?:rcx \(sid\?\):|sid:)\s*([0-9A-Fa-f]{16})\s+\(([^)]+)\).*?(?:func_ptr:|at address:)\s*([0-9A-Fa-f]{16})"
)


def main() -> None:
    in_path = sys.argv[1] if len(sys.argv) > 1 else "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/dconstruct_log.txt"
    out_path = sys.argv[2] if len(sys.argv) > 2 else "sid_addresses.tsv"

    addresses = {}
    with open(in_path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            match = LOG_RE.search(line)
            if match:
                sid, name, address = match.groups()
                addresses[(sid.upper(), name)] = address.upper()

    with open(out_path, "w", encoding="utf-8", newline="\n") as f:
        for (sid, name), address in sorted(addresses.items()):
            f.write(f"{sid}\t{name}\t{address}\n")

    print(f"wrote {len(addresses)} entries to {out_path}")


if __name__ == "__main__":
    main()
