def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mult = 1099511628211
    for row in rows:
        u = s = f = None
        for pair in row.split('&'):
            if not pair:
                continue
            idx = pair.find('=')
            if idx == -1:
                continue
            key = pair[:idx]
            val = pair[idx+1:]
            if key == 'u':
                u = int(val)
            elif key == 'score':
                s = int(val)
            elif key == 'flag':
                f = int(val)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mult) & mask
    return h
