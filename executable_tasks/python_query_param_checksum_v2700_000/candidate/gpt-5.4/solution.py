MASK64 = (1 << 64) - 1
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def _parse_int_after_key(row: str, key: str) -> int:
    i = row.find(key)
    if i < 0:
        raise KeyError(key[:-1])
    i += len(key)
    n = len(row)
    v = 0
    while i < n:
        c = ord(row[i])
        if 48 <= c <= 57:
            v = v * 10 + (c - 48)
            i += 1
        else:
            break
    return v


def run(rows: list[str]) -> int:
    h = FNV_OFFSET
    for row in rows:
        u = _parse_int_after_key(row, "u=")
        s = _parse_int_after_key(row, "score=")
        f = _parse_int_after_key(row, "flag=")
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * FNV_PRIME) & MASK64
    return h
