def parse_int_at(s: str, start: int) -> int:
    v = 0
    while start < len(s) and "0" <= s[start] <= "9":
        v = v * 10 + (ord(s[start]) - 48)
        start += 1
    return v


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u_pos = row.index('"u":') + 4
        s_pos = row.index('"s":') + 4
        u = parse_int_at(row, u_pos)
        s = parse_int_at(row, s_pos)
        f = '"f":true' in row
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
