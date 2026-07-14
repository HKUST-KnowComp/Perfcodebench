def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for row in rows:
        u = s = f = 0
        for kv in row.split('&'):
            k, sep, v_str = kv.partition('=')
            if not sep:
                continue
            if k == 'u':
                u = int(v_str)
            elif k == 'score':
                s = int(v_str)
            elif k == 'flag':
                f = int(v_str)
        v = (u & 2047) + s + f * 19
        h = ((h ^ (v + 1)) * prime) & mask
    return h