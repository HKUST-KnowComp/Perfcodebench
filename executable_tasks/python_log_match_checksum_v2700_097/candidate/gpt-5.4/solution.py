PAT_LEN = 15
MASK64 = (1 << 64) - 1
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def _is_match(s: str) -> bool:
    if len(s) != PAT_LEN:
        return False

    c0 = ord(s[0])
    c1 = ord(s[1])
    if c0 < 65 or c0 > 90 or c1 < 65 or c1 > 90:
        return False

    c2 = ord(s[2])
    c3 = ord(s[3])
    c4 = ord(s[4])
    c5 = ord(s[5])
    if not (48 <= c2 <= 57 and 48 <= c3 <= 57 and 48 <= c4 <= 57 and 48 <= c5 <= 57):
        return False

    if s[6] != '-':
        return False

    c7 = ord(s[7])
    c8 = ord(s[8])
    c9 = ord(s[9])
    c10 = ord(s[10])
    c11 = ord(s[11])
    c12 = ord(s[12])
    c13 = ord(s[13])
    c14 = ord(s[14])

    return (
        (48 <= c7 <= 57 or 65 <= c7 <= 70) and
        (48 <= c8 <= 57 or 65 <= c8 <= 70) and
        (48 <= c9 <= 57 or 65 <= c9 <= 70) and
        (48 <= c10 <= 57 or 65 <= c10 <= 70) and
        (48 <= c11 <= 57 or 65 <= c11 <= 70) and
        (48 <= c12 <= 57 or 65 <= c12 <= 70) and
        (48 <= c13 <= 57 or 65 <= c13 <= 70) and
        (48 <= c14 <= 57 or 65 <= c14 <= 70)
    )


def run(rows: list[str]) -> int:
    h = FNV_OFFSET
    prime = FNV_PRIME
    mask = MASK64

    for row in rows:
        if _is_match(row):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h = (h * prime) & mask

    return h
