from urllib.parse import unquote_plus


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    unquote = unquote_plus  # local reference for faster access
    for row in rows:
        u_val = s_val = f_val = None
        for pair in row.split('&'):
            key, _, val = pair.partition('=')
            if key == 'u':
                u_val = val
            elif key == 'score':
                s_val = val
            elif key == 'flag':
                f_val = val
        # unquote only when necessary; otherwise int() handles plain digits natively
        if '%' in u_val or '+' in u_val:
            u_val = unquote(u_val)
        if '%' in s_val or '+' in s_val:
            s_val = unquote(s_val)
        if '%' in f_val or '+' in f_val:
            f_val = unquote(f_val)
        u = int(u_val)
        s = int(s_val)
        f = int(f_val)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= mult
        h &= mask
    return h
