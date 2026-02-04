import re


def is_hex(str: str) -> bool:
    try:
        num = int(str, 16)
        return True
    except:
        return False


with open(r"C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\source\hooking\tlou-ii.exe.txt", "r+") as f:
    new_lines = []
    for line in f.readlines():

        if line.strip().startswith(";") or all(is_hex(token) for token in line.split(",")):
            continue
        if not line.startswith(".text"):
            new_lines.append(line)
            continue
        new_line = line.split()[2:]
        line_str = "    " + " ".join(new_line)
        # 1) convert 0xABC -> ABCh
        line_str = re.sub(r"\b0x([0-9A-Fa-f]+)\b", r"\1h", line_str)

        # 2) convert bare hex numbers -> <number>h
        #    skip ones already ending with h
        line_str = re.sub(r"\b([0-9A-Fa-f]+)\b(?!h)", r"\1h", line_str)
        new_lines.append(line_str)

    f.seek(0)
    f.truncate()
    f.write(".code\np PROC\n")
    f.write("\n".join(new_lines))
    f.write("\np ENDPROC\nEND")
