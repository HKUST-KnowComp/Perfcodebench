def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211
    for row in rows:
        u = None
        s = None
        fl = None
        for part in row.split('&'):
            if u is not None and s is not None and fl is not None:
                break
            k, sep, v = part.partition('=')
            if sep == '':
                # Ignore items without '=' (strict_parsing=False behavior)
                continue
            if v == '':
                # Skip blank values to match keep_blank_values=False
                continue
            if u is None and k == 'u':
                u = int(v)
                continue
            if s is None and k == 'score':
                s = int(v)
                continue
            if fl is None and k == 'flag':
                fl = int(v)
                continue
        vv = (u & 2047) + s + fl * 19
        h ^= vv + 1
        h = (h * mul) & mask
    return h
