def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for row in rows:
        vals = {}
        for part in row.split('&'):
            if '=' in part:
                k, v = part.split('=', 1)
                if k not in vals:
                    vals[k] = v
        u = int(vals['u'])
        s = int(vals['score'])
        f = int(vals['flag'])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mult) & mask
    return h
