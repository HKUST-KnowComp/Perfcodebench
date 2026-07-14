import re

# Pre-compiled regexes that match the three fields we need.
# They are anchored to the first occurrence of the key and capture the value.
_RE_U = re.compile(r'"u"\s*:\s*(\d+)')
_RE_S = re.compile(r'"s"\s*:\s*(-?\d+)')
_RE_F = re.compile(r'"f"\s*:\s*(true|false)')

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Extract u (unsigned), s (signed), f (bool)
        u = int(_RE_U.search(row).group(1))
        s = int(_RE_S.search(row).group(1))
        f = _RE_F.search(row).group(1) == "true"

        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h