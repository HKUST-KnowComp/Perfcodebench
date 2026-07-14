import string

# Pre-computed lookup tables for fast classification
def _build_tables():
    upper = {c for c in string.ascii_uppercase}
    digit  = {c for c in string.digits}
    hex_   = digit | {c for c in string.hexdigits[10:16]}  # A-F
    return upper, digit, hex_

_UPPER, _DIGIT, _HEX = _build_tables()

# Bit-mask for 64-bit wrap
_MASK = (1 << 64) - 1

# Constants for FNV-1a
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME  = 1099511628211

def run(rows: list[str]) -> int:
    h = _FNV_OFFSET
    for row in rows:
        if len(row) != 14:
            v = 0
        else:
            # Pattern == ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
            if (
                row[0] in _UPPER and row[1] in _UPPER and
                row[2] in _DIGIT and row[3] in _DIGIT and
                row[4] in _DIGIT and row[5] in _DIGIT and
                row[6] == '-' and
                row[7]  in _HEX and row[8]  in _HEX and
                row[9]  in _HEX and row[10] in _HEX and
                row[11] in _HEX and row[12] in _HEX and
                row[13] in _HEX and row[14] in _HEX
            ):
                v = ord(row[2]) + ord(row[7])
            else:
                v = 0
        h ^= v + 1
        h *= _FNV_PRIME
        h &= _MASK
    return h