from urllib.parse import unquote_plus


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    unq = unquote_plus
    int_ = int

    for row in rows:
        u = s = f = None
        for param in row.split('&'):
            key, sep, val = param.partition('=')
            if not sep:
                continue
            if key == 'u' and u is None:
                u = int_(unq(val))
            elif key == 'score' and s is None:
                s = int_(unq(val))
            elif key == 'flag' and f is None:
                f = int_(unq(val))
            if u is not None and s is not None and f is not None:
                break
        # This line assumes all three values were found (as in the original),
        # otherwise it will raise an error trying to add None.
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mult) & mask

    return h
