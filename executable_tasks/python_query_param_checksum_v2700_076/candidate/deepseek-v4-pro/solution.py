def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for row in rows:
        parts = row.split('&')
        u = s = f = None
        for part in parts:
            if u is None and part.startswith('u='):
                u = int(part[2:])
            elif s is None and part.startswith('score='):
                s = int(part[6:])
            elif f is None and part.startswith('flag='):
                f = int(part[5:])
            if u is not None and s is not None and f is not None:
                break
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mult) & mask
    return h
