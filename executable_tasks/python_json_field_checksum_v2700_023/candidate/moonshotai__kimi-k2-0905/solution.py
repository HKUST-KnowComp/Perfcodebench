import re

# Pre-compiled regex that captures the three fields we need:
#   u : integer after '"u":'
#   s : integer after '"s":'
#   f : boolean after '"f":' (true|false)
# The regex is anchored to run left-to-right and stops as soon as all three are found.
_row_re = re.compile(
    r'.*?'                       # non-greedy skip until we hit our keys
    r'"u":(-?\d+).*?'           # capture integer u
    r'"s":(-?\d+).*?'           # capture integer s
    r'"f":(true|false)'         # capture boolean f
    , re.DOTALL | re.IGNORECASE
)

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Local bindings to avoid attribute lookup in the tight loop
    search = _row_re.search
    for row in rows:
        m = search(row)
        # Extract fields directly as strings; convert to int/bool ourselves
        u = int(m.group(1))
        s = int(m.group(2))
        f = m.group(3).lower() == 'true'
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h