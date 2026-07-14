import re

# one-shot regex: capture u, s, and the literal "f":true/false
#  - u is captured as digits (possibly negative)
#  - s is captured as digits (possibly negative)
#  - f is captured as the single char 't' or 'f' (true/false)
_row_re = re.compile(r'"u":(-?\d+).*"s":(-?\d+).*"f":(t|f)')

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        m = _row_re.search(row)
        # convert captured groups
        u = int(m.group(1))
        s = int(m.group(2))
        f = m.group(3) == 't'          # True if "f":true
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h