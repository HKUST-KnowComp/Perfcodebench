def run(rows: list[str]) -> int:
    # FNV-1a-like parameters
    h = 1469598103934665603
    mul = 1099511628211
    mask = (1 << 64) - 1

    # Prebind for speed
    A = 'A'; Z = 'Z'
    ZERO = '0'; NINE = '9'
    F = 'F'
    HYPHEN = '-'
    o = ord

    for row in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$  => length 15
        if len(row) == 15:
            c0 = row[0]; c1 = row[1]
            if A <= c0 <= Z and A <= c1 <= Z:
                c2 = row[2]; c3 = row[3]; c4 = row[4]; c5 = row[5]
                if ZERO <= c2 <= NINE and ZERO <= c3 <= NINE and ZERO <= c4 <= NINE and ZERO <= c5 <= NINE:
                    if row[6] == HYPHEN:
                        d7 = row[7]; d8 = row[8]; d9 = row[9]; d10 = row[10]
                        d11 = row[11]; d12 = row[12]; d13 = row[13]; d14 = row[14]
                        if (
                            ((ZERO <= d7 <= NINE) or (A <= d7 <= F)) and
                            ((ZERO <= d8 <= NINE) or (A <= d8 <= F)) and
                            ((ZERO <= d9 <= NINE) or (A <= d9 <= F)) and
                            ((ZERO <= d10 <= NINE) or (A <= d10 <= F)) and
                            ((ZERO <= d11 <= NINE) or (A <= d11 <= F)) and
                            ((ZERO <= d12 <= NINE) or (A <= d12 <= F)) and
                            ((ZERO <= d13 <= NINE) or (A <= d13 <= F)) and
                            ((ZERO <= d14 <= NINE) or (A <= d14 <= F))
                        ):
                            v = o(c2) + o(d7)
        h ^= v + 1
        h = (h * mul) & mask
    return h
