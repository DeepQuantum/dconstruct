import argparse
import pathlib
import re
import struct
import sys


DEFAULT_SIDBASE = pathlib.Path("test/fixtures/dc/sidbase_new_fixed.bin")
QUAD_RE = re.compile(r"^(?P<prefix>\s*\.quad\s+)(?P<value>[+-]?(?:0x[0-9A-Fa-f]+|\d+))(?P<rest>\s*(?:;.*)?)$")


def load_sidbase(path: pathlib.Path) -> dict[int, str]:
    data = path.read_bytes()
    if len(data) < 8:
        raise ValueError(f"{path} is too small to be a sidbase")

    entry_count = struct.unpack_from("<Q", data, 0)[0]
    hashes: dict[int, str] = {}
    table_offset = 8

    for index in range(entry_count - 1):
        entry_offset = table_offset + index * 16
        if entry_offset + 16 > len(data):
            raise ValueError(f"{path} has a truncated sid table at entry {index}")

        hash_value, string_offset = struct.unpack_from("<QQ", data, entry_offset)
        if string_offset >= len(data):
            continue

        end = data.find(b"\x00", string_offset)
        if end == -1:
            end = len(data)
        hashes[hash_value] = data[string_offset:end].decode("utf-8", errors="replace")

    return hashes


def parse_quad_value(text: str) -> int:
    return int(text, 0) & 0xFFFFFFFFFFFFFFFF


def annotate_line(line: str, sidbase: dict[int, str], replace_existing: bool) -> tuple[str, bool]:
    newline = "\n" if line.endswith("\n") else ""
    body = line[:-1] if newline else line
    match = QUAD_RE.match(body)
    if match is None:
        return line, False

    rest = match.group("rest")
    if rest.strip().startswith(";") and not replace_existing:
        return line, False

    hash_value = parse_quad_value(match.group("value"))
    name = sidbase.get(hash_value)
    comment = f"; sidbase: 0x{hash_value:016X}"
    if name is not None:
        comment += f" {name}"

    return f"{match.group('prefix')}{match.group('value')} {comment}{newline}", True


def annotate_text(text: str, sidbase: dict[int, str], replace_existing: bool) -> tuple[str, int]:
    changed = 0
    out_lines = []
    for line in text.splitlines(keepends=True):
        annotated, did_change = annotate_line(line, sidbase, replace_existing)
        out_lines.append(annotated)
        changed += int(did_change)
    return "".join(out_lines), changed


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Annotate numeric .quad lines in a DCVM .s file with unsigned hash values and sidbase names."
    )
    parser.add_argument("asm", type=pathlib.Path, help="input .s file")
    parser.add_argument(
        "--sidbase",
        type=pathlib.Path,
        default=DEFAULT_SIDBASE,
        help=f"sidbase binary path (default: {DEFAULT_SIDBASE})",
    )
    parser.add_argument("-o", "--output", type=pathlib.Path, help="write annotated output to this file")
    parser.add_argument("--in-place", action="store_true", help="rewrite the input file")
    parser.add_argument(
        "--replace-existing",
        action="store_true",
        help="replace existing comments on .quad lines instead of leaving them unchanged",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.in_place and args.output is not None:
        print("error: use either --in-place or --output, not both", file=sys.stderr)
        return 2

    sidbase_path = args.sidbase
    if not sidbase_path.is_absolute():
        sidbase_path = pathlib.Path.cwd() / sidbase_path

    sidbase = load_sidbase(sidbase_path)
    text = args.asm.read_text(encoding="utf-8", errors="replace")
    annotated, changed = annotate_text(text, sidbase, args.replace_existing)

    if args.in_place:
        args.asm.write_text(annotated, encoding="utf-8", newline="")
    elif args.output is not None:
        args.output.write_text(annotated, encoding="utf-8", newline="")
    else:
        sys.stdout.write(annotated)

    print(f"annotated {changed} .quad lines using {len(sidbase)} sidbase entries", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
