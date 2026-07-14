import re

# Pre-compiled regex to extract the three fields from a JSON object line.
# The pattern looks for "u": followed by a number, "s": followed by a number,
# and "f": followed by either true or false (case-insensitive).
_parser = re.compile(
    r'"u":(\d+).+?"s":(-?\d+).+?"f":(true|false)',
    flags=re.IGNORECASE
)


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask1023 = 1023
    m1 = 1099511628211
    m2 = (1 << 64) - 1

    # Local bindings for speed
    local_parser = _parser
    local_int = int
    local_hash = h

    for row in rows:
        m = local_parser.search(row)
        u = local_int(m.group(1))
        s = local_int(m.group(2))
        f = m.group(3)[0] in {'t', 'T'}  # true starts with 't' or 'T'
        v = (u & mask1023) + s + (17 if f else 0)
        local_hash ^= v + 1
        local_hash *= m1
        local_hash &= m2

    return local_hash