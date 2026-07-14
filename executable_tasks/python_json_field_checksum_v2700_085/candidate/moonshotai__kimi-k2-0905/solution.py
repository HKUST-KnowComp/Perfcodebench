import re

# Pre-compiled regex that captures the three fields we care about:
#   u : integer after '"u":'
#   s : integer after '"s":'
#   f : boolean after '"f":'
# The pattern is tight and anchored so the engine quits as soon as it succeeds.
_field_re = re.compile(
    r'.*?'                 # lazily skip to first field
    r'"u":(-?\d+).*?'     # capture u
    r'"s":(-?\d+).*?'     # capture s
    r'"f":(true|false)'   # capture f
    , re.DOTALL
)

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        m = _field_re.match(row)
        # Extract fields as strings, convert to int/bool
        u = int(m.group(1))
        s = int(m.group(2))
        f = m.group(3) == "true"
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h