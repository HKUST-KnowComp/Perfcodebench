import re

# tuple of 10-character upper-cased strings that have the required pattern
_ALLOWED_TAIL = tuple(
    "{:04d}-{:s}".format(i, h)
    for i in range(10000)
    for h in ['{:08X}'.format(j) for j in range(0x10000000, 0x100000000, 0x10000000)]
)

def _parse_quick(row: str) -> bool:
    """Return True if row is ‘XX1234-ABCDEFGH’ (case-insensitive)."""
    return (
        len(row) == 15
        and 'A' <= row[0] <= 'Z'
        and 'A' <= row[1] <= 'Z'
        and row[6] == '-'
        and row[2:6].isdigit()
        and row[7:] in _ALLOWED_TAIL
    )


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if _parse_quick(row):
            v = ord(row[2]) + ord(row[7])
            h ^= v + 1
            h *= 1099511628211
            h &= (1 << 64) - 1
        else:
            h ^= 0 + 1
            h *= 1099511628211
            h &= (1 << 64) - 1
    return h