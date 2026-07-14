def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s_val = f_val = None
        for part in row.split('&'):
            key, sep, val = part.partition('=')
            if sep:
                if not val:
                    continue
                if key == 'u':
                    u = val
                elif key == 'score':
                    s_val = val
                elif key == 'flag':
                    f_val = val
            if u is not None and s_val is not None and f_val is not None:
                break
        v = (int(u) & 2047) + int(s_val) + int(f_val) * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
