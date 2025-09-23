import sys
import re


def main():
    if len(sys.argv) < 2:
        print("error: not enough args")
    file = sys.argv[1]

    header = """
#define function int
#define unknown int

"""

    with open(file, "r") as f:
        contents = f.read()
        contents = re.sub(r"(\w)-(\w)", r"\1_\2", contents)
        contents = re.sub(r"#(\w*)", r"_\1", contents)
        contents = contents.replace("?", "")

    with open(file.replace(".dcpl", ".c"), "w+") as f:
        f.write(header + contents)


if __name__ == "__main__":
    main()
