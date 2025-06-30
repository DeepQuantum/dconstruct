import datetime
import os
import sys


def main():
    compiler, version = sys.argv[1].split(";")
    with open("buildinfo.h", "w+") as f:
        f.write("#pragma once\n")
        f.write(f'#define COMPILER_VERSION "{compiler}"\n')
        f.write(f'#define BUILD_DATE "{datetime.datetime.now().ctime()}"\n')
        f.write(f'#define VERSION "{version}"\n')


if __name__ == "__main__":
    main()
