from urllib.parse import parse_qs


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = None
        for param in row.split('&'):
            key, value = param.split('=', 1)
            if key == 'u':
                u = int(value)
            elif key == 'score':
                s = int(value)
            elif key == 'flag':
                f = int(value)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h