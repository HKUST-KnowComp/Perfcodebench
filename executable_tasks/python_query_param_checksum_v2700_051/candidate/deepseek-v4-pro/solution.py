from urllib.parse import unquote

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    for row in rows:
        u_val = None
        s_val = None
        f_val = None
        for part in row.split('&'):
            if not part:
                continue
            if '=' not in part:
                continue
            key, val = part.split('=', 1)
            if key == 'u' and u_val is None:
                u_val = int(unquote(val))
            elif key == 'score' and s_val is None:
                s_val = int(unquote(val))
            elif key == 'flag' and f_val is None:
                f_val = int(unquote(val))
        if u_val is None or s_val is None or f_val is None:
            raise KeyError
        v = (u_val & 2047) + s_val + f_val * 19
        h ^= v + 1
        h = (h * multiplier) & mask
    return h
