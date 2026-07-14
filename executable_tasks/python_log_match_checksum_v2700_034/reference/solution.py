def is_hex(c: str) -> bool:
    return ("0" <= c <= "9") or ("A" <= c <= "F")


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for s in rows:
        ok = (
            len(s) == 15
            and "A" <= s[0] <= "Z"
            and "A" <= s[1] <= "Z"
            and "0" <= s[2] <= "9"
            and "0" <= s[3] <= "9"
            and "0" <= s[4] <= "9"
            and "0" <= s[5] <= "9"
            and s[6] == "-"
            and is_hex(s[7]) and is_hex(s[8]) and is_hex(s[9]) and is_hex(s[10])
            and is_hex(s[11]) and is_hex(s[12]) and is_hex(s[13]) and is_hex(s[14])
        )
        v = (ord(s[2]) + ord(s[7])) if ok else 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
