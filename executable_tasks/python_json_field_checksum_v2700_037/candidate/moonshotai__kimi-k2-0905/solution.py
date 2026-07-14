import re

# Pre-compiled regexes that match the three fields we need.
# They are anchored to the first occurrence of each key and capture the value.
_U_RE = re.compile(br'"u"\s*:\s*(\d+)')
_S_RE = re.compile(br'"s"\s*:\s*(-?\d+)')
_F_RE = re.compile(br'"f"\s*:\s*(true|false)')

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        b = row.encode('utf-8')          # bytes for regex speed
        u = int(_U_RE.search(b).group(1))
        s = int(_S_RE.search(b).group(1))
        f = _F_RE.search(b).group(1) == b'true'
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h