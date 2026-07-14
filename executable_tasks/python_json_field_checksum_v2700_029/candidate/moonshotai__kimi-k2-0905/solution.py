import re

# Pre-compiled regexes that match the three fields we need.
# They are anchored to avoid accidental partial matches.
_u_re = re.compile(br'"u"\s*:\s*(\d+)')
_s_re = re.compile(br'"s"\s*:\s*(-?\d+)')
_f_re = re.compile(br'"f"\s*:\s*(true|false)')


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        b = row.encode('utf-8')

        # Extract u
        m = _u_re.search(b)
        u = int(m.group(1)) & 1023

        # Extract s
        m = _s_re.search(b)
        s = int(m.group(1))

        # Extract f
        m = _f_re.search(b)
        f = m.group(1) == b'true'

        v = u + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h