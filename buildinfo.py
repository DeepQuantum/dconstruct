import datetime
import os
import sys


def main():
    with open("buildinfo.h", "w+") as f:
        f.write("#pragma once\n")
        f.write(f'#define COMPILER_VERSION "{sys.argv[1]}"\n')
        f.write(f'#define BUILD_DATE "{datetime.datetime.now().ctime()}"\n')


if __name__ == "__main__":
    main()
