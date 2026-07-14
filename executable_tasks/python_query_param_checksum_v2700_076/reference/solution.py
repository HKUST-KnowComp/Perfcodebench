def parse_int_at(s: str, start: int) -> int:
    v = 0
    while start < len(s) and "0" <= s[start] <= "9":
        v = v * 10 + (ord(s[start]) - 48)
        start += 1
    return v


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        up = row.index("u=") + 2
        sp = row.index("&score=") + 7
        fp = row.index("&flag=") + 6
        u = parse_int_at(row, up)
        s = parse_int_at(row, sp)
        f = parse_int_at(row, fp)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
