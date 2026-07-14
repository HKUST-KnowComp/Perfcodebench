def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xffffffffffffffff
    mul = 1099511628211
    for row in rows:
        u = None
        s = None
        f = None
        for part in row.split('&'):
            if part.startswith('u='):
                if u is None:
                    u = int(part[2:])
            elif part.startswith('score='):
                if s is None:
                    s = int(part[6:])
            elif part.startswith('flag='):
                if f is None:
                    f = int(part[5:])
            if u is not None and s is not None and f is not None:
                break
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mul) & mask
    return h