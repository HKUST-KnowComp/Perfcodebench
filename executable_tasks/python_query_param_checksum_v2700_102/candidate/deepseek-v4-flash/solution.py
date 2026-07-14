from urllib.parse import unquote_plus

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    const = 1099511628211
    for row in rows:
        u = s = f = None
        for pair in row.split('&'):
            if '=' not in pair:
                continue
            key, value = pair.split('=', 1)
            if key == 'u':
                u = int(unquote_plus(value))
            elif key == 'score':
                s = int(unquote_plus(value))
            elif key == 'flag':
                f = int(unquote_plus(value))
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= const
        h &= mask
    return h