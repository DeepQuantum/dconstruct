from __future__ import annotations

import re
from collections import Counter
from pathlib import Path

REGEX_PATTERN = r"m_imLazyPt2\s*= \[0x[0-9A-Fa-f]+\] u64\[1\] { (.*?) }"

REGEX_FLAGS = re.MULTILINE | re.DOTALL

SEARCH_DIR = Path("./disassembled")

CASE_INSENSITIVE = False


def main() -> None:
    if not SEARCH_DIR.exists() or not SEARCH_DIR.is_dir():
        print(f"Search directory not found: {SEARCH_DIR.resolve()}")
        return

    flags = REGEX_FLAGS | (re.IGNORECASE if CASE_INSENSITIVE else 0)
    try:
        pattern = re.compile(REGEX_PATTERN, flags)
    except re.error as exc:
        print(f"Invalid regex pattern: {exc}")
        return

    counts: Counter[str] = Counter()
    scanned_files = 0

    for file_path in SEARCH_DIR.rglob("*"):
        if not file_path.is_file():
            continue

        scanned_files += 1
        try:
            content = file_path.read_text(encoding="utf-8", errors="ignore")
        except OSError as exc:
            print(f"Skipping {file_path}: {exc}")
            continue

        for match in pattern.finditer(content):
            counts[match.group(1)] += 1

    print(f"Scanned files: {scanned_files}")
    print(f"Distinct matches: {len(counts)}")
    print(f"Total matches: {sum(counts.values())}\n")

    if not counts:
        print("No matches found.")
        return

    print("Matches (grouped):")
    for value, count in sorted(counts.items(), key=lambda item: (-item[1], item[0])):
        print(f"{count:>8}  {value}")


if __name__ == "__main__":
    main()
