def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = None
        for part in row.split('&'):
            if not part:
                continue
            if part.startswith('u='):
                if u is None: u = int(part[2:])
            elif part.startswith('score='):
                if s is None: s = int(part[6:])
            elif part.startswith('flag='):
                if f is None: f = int(part[5:])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h