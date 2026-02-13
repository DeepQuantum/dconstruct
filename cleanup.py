from concurrent.futures import ThreadPoolExecutor, as_completed
import os
import re
from typing import Callable
import pathlib

DECOMP_PATH = pathlib.Path(r"./disassembled")


def replace_boxed_value(txt: str) -> str:
    """
    new-boxed-value(7, ...) -> boxed_symbol(...)
    """

    BOXED_VALUE_IDS: tuple = (
        "invalid",
        "bool",
        "i32",
        "u32",
        "i64",
        "u64",
        "f32",
        "symbol",
        "timeframePOD",
        "unused",
        "netTrackerId",
        "processHandleMutable",
        "processHandle",
        "p64",
        "point",
        "vector",
        "quat",
        "locator",
        "boundframe",
        "stringcopy",
        "unused",
        "array",
        "color",
        "ssAnimateParams",
        "NDattackInfo",
    )
    BOXED_VALUE_RE: re.Pattern = re.compile(r"(new-boxed-value\()(?P<boxed_value_num>\d+), (?P<boxed_val>.*?)(\))")

    def repl(m: re.Match) -> str:
        return f"boxed_{BOXED_VALUE_IDS[int(m.group("boxed_value_num"))]}({m.group("boxed_val")})"

    res = BOXED_VALUE_RE.sub(repl, txt)
    return res


def replace_foreach_check(txt: str) -> str:
    FOREACH_CHECK_RE: re.Pattern = re.compile(
        r"(?P<head>(foreach \([a-z\d]*\? var_(?P<foreach_var>\d+) : .*?\)) \{)"
        r"\s*(?P<full_body>u16 var_(?P<decl_var>\d+);"
        r"\s*if \(var_(?P<if1_cond_var>\d+) && \*\(u16\*\)\(var_(?P<if1_cond_var2>\d+) \+ 12\) == 7\) \{"
        r"\s*var_(?P<if1_assign_lhs>\d+) = \*\(u64\*\)var_(?P<if1_assign_rhs>\d+);\s*\}"
        r"\s*else if \(var_(?P<if2_cond_var>\d+) && \*\(u16\*\)\(var_(?P<if2_cond_var2>\d+) \+ 12\) == 5\) \{"
        r"\s*var_(?P<if2_assign_lhs>\d+) = \*\(u64\*\)var_(?P<if2_assign_rhs>\d+);\s*\}"
        r"\s*else if \(var_(?P<if3_cond_var>\d+) && \*\(u16\*\)\(var_(?P<if3_cond_var2>\d+) \+ 12\) == 4\) \{"
        r"\s*var_(?P<if3_assign_lhs>\d+) = \*\(u64\*\)var_(?P<if3_assign_rhs>\d+);\s*\}"
        r"\s*else \{\s*var_(?P<else_assign_lhs>\d+) = 0;\s*\})"
    )

    def repl(m: re.Match) -> str:
        head = m.group("head")
        foreach_var = m.group("foreach_var")
        decl_var = m.group("decl_var")
        return head.replace(f"var_{foreach_var}", f"var_{decl_var}")

    return FOREACH_CHECK_RE.sub(repl, txt)


def replace_array_dereferences(txt: str) -> str:
    """
    *array_var + (index * 8) -> array_var[index]
    """

    ARRAY_RE: re.Pattern = re.compile(r"\(\*(?P<array_var>var_\d+) \+ \((?P<index>\d+) \* \d+\)\)")

    def repl(m: re.Match) -> str:
        return f"{m.group("array_var")}[{m.group("index")}]"

    return ARRAY_RE.sub(repl, txt)


REPLACE_FUNCS: list[Callable[[str], str]] = [
    replace_foreach_check,
    replace_boxed_value,
    replace_array_dereferences,
]


def run_replacements(file_text: str) -> str:
    for func in REPLACE_FUNCS:
        file_text = func(file_text)
    return file_text


def process_file(full_path: str) -> None:
    with open(full_path, "r+") as f:
        text = f.read()
        new_text = run_replacements(text)
        if new_text != text:
            f.seek(0)
            f.write(new_text)
            f.truncate()


def main() -> None:
    paths = []

    for root, _, files in os.walk(DECOMP_PATH):
        for name in files:
            if name.endswith(".dcpl"):
                paths.append(os.path.join(root, name))

    with ThreadPoolExecutor() as executor:
        futures = [executor.submit(process_file, p) for p in paths]

        for f in as_completed(futures):
            f.result()


if __name__ == "__main__":
    main()
