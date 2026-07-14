import re

# Compiled patterns for extracting fields
_RE_U = re.compile(r'"u"\s*:\s*(-?\d+)')
_RE_S = re.compile(r'"s"\s*:\s*(-?\d+)')
_RE_F = re.compile(r'"f"\s*:\s*(true|false)')


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for row in rows:
        # Extract u
        m = _RE_U.search(row)
        if m is None:
            raise ValueError("Missing key 'u'")
        u = int(m.group(1))
        # Extract s
        m = _RE_S.search(row)
        if m is None:
            raise ValueError("Missing key 's'")
        s = int(m.group(1))
        # Extract f
        m = _RE_F.search(row)
        if m is None:
            raise ValueError("Missing key 'f'")
        f = (m.group(1) == "true")
        
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= mult
        h &= mask
    return h
