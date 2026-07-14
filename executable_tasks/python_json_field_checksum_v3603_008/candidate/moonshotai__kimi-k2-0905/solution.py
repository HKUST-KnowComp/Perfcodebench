import re

# Pre-compiled regex to grab the three fields we care about.
#  - u is captured as a decimal integer (may be negative)
#  - s is captured as a decimal integer (may be negative)
#  - f is captured as either true or false
_row_re = re.compile(
    r'"u"\s*:\s*(-?\d+).*?"s"\s*:\s*(-?\d+).*?"f"\s*:\s*(true|false)',
    re.DOTALL,
)


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        m = _row_re.search(row)
        if not m:  # should never happen on valid input
            raise ValueError("row missing required fields")
        u = int(m.group(1)) & 1023
        s = int(m.group(2))
        f = 17 if m.group(3) == "true" else 0
        v = u + s + f
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h