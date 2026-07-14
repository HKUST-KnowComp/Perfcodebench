def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    for row in rows:
        u = s = f = 0
        for part in row.split('&'):
            key, _, val = part.partition('=')
            if key == 'u':
                u = int(val)
            elif key == 'score':
                s = int(val)
            elif key == 'flag':
                f = int(val)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * MULT) & MASK
    return h