def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        for param in row.split('&'):
            key, eq, val = param.partition('=')
            if eq:
                if key == 'u':
                    u = int(val)
                elif key == 'score':
                    s = int(val)
                elif key == 'flag':
                    f = int(val)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
