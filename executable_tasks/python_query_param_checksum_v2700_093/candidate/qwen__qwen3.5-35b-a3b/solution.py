def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        params = {k: v for k, v in (part.split('=', 1) for part in row.split('&'))}
        u = int(params['u'])
        s = int(params['score'])
        f = int(params['flag'])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h