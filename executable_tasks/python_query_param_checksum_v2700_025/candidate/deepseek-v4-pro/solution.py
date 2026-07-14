def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for row in rows:
        u = s = f = None
        for part in row.split('&'):
            key, sep, val = part.partition('=')
            if key == 'u':
                if not val:
                    raise IndexError('list index out of range')
                u = int(val)
            elif key == 'score':
                if not val:
                    raise IndexError('list index out of range')
                s = int(val)
            elif key == 'flag':
                if not val:
                    raise IndexError('list index out of range')
                f = int(val)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= mult
        h &= mask
    return h
