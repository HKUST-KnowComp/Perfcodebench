import sys

def _validate_and_extract(row: str) -> int:
    """
    Return (ord(row[2]) + ord(row[7])) if the row matches
    ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$ , else 0.
    All checks are done with fast ASCII-range comparisons, no regex.
    """
    if len(row) != 16:                 # total length
        return 0
    if row[2] != row[7]:               # quick early exit if positions differ
        return 0

    # positions 0,1  must be uppercase letters
    c0 = row[0]
    c1 = row[1]
    if (c0 < 'A' or c0 > 'Z') or (c1 < 'A' or c1 > 'Z'):
        return 0

    # positions 2,3,4,5  must be digits
    for i in (2, 3, 4, 5):
        c = row[i]
        if c < '0' or c > '9':
            return 0

    # position 6 must be hyphen
    if row[6] != '-':
        return 0

    # positions 8..15 must be hex digits
    for i in range(8, 16):
        c = row[i]
        if not (('0' <= c <= '9') or ('A' <= c <= 'F')):
            return 0

    # All checks passed; compute the required value
    return ord(row[2]) + ord(row[7])


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        v = _validate_and_extract(row)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h