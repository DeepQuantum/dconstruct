def fnv(msg: str) -> int:
    base = 0x811C9DC5
    mask = 2**32 - 1
    for char in msg:
        base ^= ord(char)
        base *= 0x01000193
        base &= mask
    return base


print(fnv("function"))
