import re

# Pre-compiled regexes for the three fields we need.
# Each pattern captures the raw text of the value (group 1).
_U_RE = re.compile(r'"u"\s*:\s*(-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)')
_S_RE = re.compile(r'"s"\s*:\s*(-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)')
_F_RE = re.compile(r'"f"\s*:\s*(true|false)')

def _parse_int_or_float(txt: str) -> int:
    """Convert JSON number literal to int; floats are truncated toward zero."""
    # Fast-path for plain integers
    if txt.isdigit() or (txt[0] == '-' and txt[1:].isdigit()):
        return int(txt)
    # General case: parse as float, then cast to int
    return int(float(txt))

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Extract the three fields
        u_match = _U_RE.search(row)
        s_match = _S_RE.search(row)
        f_match = _F_RE.search(row)
        if u_match is None or s_match is None or f_match is None:
            # Malformed row; mimic json.loads() raising, but we never expect this
            raise ValueError("missing required field")

        u = _parse_int_or_float(u_match.group(1)) & 1023
        s = _parse_int_or_float(s_match.group(1))
        f = 17 if f_match.group(1) == "true" else 0

        v = u + s + f
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h