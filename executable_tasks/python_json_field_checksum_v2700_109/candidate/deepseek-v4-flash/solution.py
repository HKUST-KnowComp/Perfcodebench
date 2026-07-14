import re

# Precompile the regex pattern for speed
_pat = re.compile(r'"(\w+)":\s*((?:-?\d+)|true|false)')

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        u = None
        s = None
        f = None
        for key, val in _pat.findall(row):
            if key == 'u':
                u = int(val)
            elif key == 's':
                s = int(val)
            elif key == 'f':
                f = (val == 'true')
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h