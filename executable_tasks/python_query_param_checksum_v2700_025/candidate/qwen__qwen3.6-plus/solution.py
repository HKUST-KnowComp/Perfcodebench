def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for row in rows:
        u = s = f = None
        for part in row.split('&'):
            k, _, v_str = part.partition('=')
            if k == 'u' and u is None:
                u = int(v_str)
            elif k == 'score' and s is None:
                s = int(v_str)
            elif k == 'flag' and f is None:
                f = int(v_str)
        v = (u & 2047) + s + f * 19
        h = ((h ^ (v + 1)) * prime) & mask
    return h