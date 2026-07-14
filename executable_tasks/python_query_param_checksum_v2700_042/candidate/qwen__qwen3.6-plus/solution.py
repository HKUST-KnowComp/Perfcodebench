def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    for row in rows:
        u = s = f = 0
        for kv in row.split('&'):
            if kv.startswith('u='):
                u = int(kv[2:])
            elif kv.startswith('score='):
                s = int(kv[6:])
            elif kv.startswith('flag='):
                f = int(kv[5:])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h