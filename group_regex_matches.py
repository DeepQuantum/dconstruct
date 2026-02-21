from __future__ import annotations

import re
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

REGEX_PATTERN = r"m_unkNumberDeclarationList\s*=\s*(.*)"

REGEX_FLAGS = re.MULTILINE

SEARCH_DIR = Path("./disassembled")

CASE_INSENSITIVE = False


def process_file(file_path: Path, pattern: re.Pattern[str]) -> Counter[str]:
    try:
        content = file_path.read_text(encoding="utf-8", errors="ignore")
    except OSError as exc:
        print(f"Skipping {file_path}: {exc}")
        return Counter()

    file_counts: Counter[str] = Counter()
    for match in pattern.finditer(content):
        file_counts[match.group(1)] += 1
    return file_counts


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

    file_paths = sorted(
        [file_path for file_path in SEARCH_DIR.rglob("*") if file_path.is_file() and file_path.suffix == ".asm"],
        key=lambda path: str(path),
    )

    counts: Counter[str] = Counter()
    file_occurrences: Counter[str] = Counter()
    first_file_for_value: dict[str, Path] = {}
    with ThreadPoolExecutor() as executor:
        for file_path, file_counts in zip(file_paths, executor.map(lambda path: process_file(path, pattern), file_paths)):
            counts.update(file_counts)
            for value in file_counts:
                file_occurrences[value] += 1
                if value not in first_file_for_value:
                    first_file_for_value[value] = file_path

    scanned_files = len(file_paths)

    print(f"Scanned files: {scanned_files}")
    print(f"Distinct matches: {len(counts)}")
    print(f"Total matches: {sum(counts.values())}\n")

    if not counts:
        print("No matches found.")
        return

    print("Matches (grouped):")
    for value, count in sorted(counts.items(), key=lambda item: (-item[1], item[0])):
        if file_occurrences[value] == 1:
            print(f"{count:>8}  {value}  {first_file_for_value[value]}")
        else:
            print(f"{count:>8}  {value}")


if __name__ == "__main__":
    main()
