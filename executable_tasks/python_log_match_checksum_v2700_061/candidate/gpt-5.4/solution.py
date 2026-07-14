PAT_LEN = 15
_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def _is_match(row: str) -> bool:
    if len(row) != PAT_LEN:
        return False

    c0 = ord(row[0])
    c1 = ord(row[1])
    if c0 < 65 or c0 > 90 or c1 < 65 or c1 > 90:
        return False

    c2 = ord(row[2])
    c3 = ord(row[3])
    c4 = ord(row[4])
    c5 = ord(row[5])
    if c2 < 48 or c2 > 57 or c3 < 48 or c3 > 57 or c4 < 48 or c4 > 57 or c5 < 48 or c5 > 57:
        return False

    if row[6] != '-':
        return False

    c7 = ord(row[7])
    c8 = ord(row[8])
    c9 = ord(row[9])
    c10 = ord(row[10])
    c11 = ord(row[11])
    c12 = ord(row[12])
    c13 = ord(row[13])
    c14 = ord(row[14])

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
    h = _FNV_OFFSET
    mask = _MASK64
    prime = _FNV_PRIME

    for row in rows:
        if len(row) == PAT_LEN:
            c0 = ord(row[0])
            c1 = ord(row[1])
            if 65 <= c0 <= 90 and 65 <= c1 <= 90:
                c2 = ord(row[2])
                c3 = ord(row[3])
                c4 = ord(row[4])
                c5 = ord(row[5])
                if 48 <= c2 <= 57 and 48 <= c3 <= 57 and 48 <= c4 <= 57 and 48 <= c5 <= 57 and row[6] == '-':
                    c7 = ord(row[7])
                    c8 = ord(row[8])
                    c9 = ord(row[9])
                    c10 = ord(row[10])
                    c11 = ord(row[11])
                    c12 = ord(row[12])
                    c13 = ord(row[13])
                    c14 = ord(row[14])
                    if (
                        (48 <= c7 <= 57 or 65 <= c7 <= 70) and
                        (48 <= c8 <= 57 or 65 <= c8 <= 70) and
                        (48 <= c9 <= 57 or 65 <= c9 <= 70) and
                        (48 <= c10 <= 57 or 65 <= c10 <= 70) and
                        (48 <= c11 <= 57 or 65 <= c11 <= 70) and
                        (48 <= c12 <= 57 or 65 <= c12 <= 70) and
                        (48 <= c13 <= 57 or 65 <= c13 <= 70) and
                        (48 <= c14 <= 57 or 65 <= c14 <= 70)
                    ):
                        v = c2 + c7
                    else:
                        v = 0
                else:
                    v = 0
            else:
                v = 0
        else:
            v = 0

        h ^= v + 1
        h = (h * prime) & mask

    return h
