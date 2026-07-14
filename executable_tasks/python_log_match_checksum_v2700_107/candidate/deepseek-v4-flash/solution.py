def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # manual validation equivalent to ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        v = 0
        if len(row) == 15:
            c0 = ord(row[0])
            c1 = ord(row[1])
            c2 = ord(row[2])
            c3 = ord(row[3])
            c4 = ord(row[4])
            c5 = ord(row[5])
            c6 = ord(row[6])
            c7 = ord(row[7])
            # check first two are uppercase
            if 65 <= c0 <= 90 and 65 <= c1 <= 90:
                # check positions 2-5 are digits
                if 48 <= c2 <= 57 and 48 <= c3 <= 57 and 48 <= c4 <= 57 and 48 <= c5 <= 57:
                    # check dash
                    if c6 == 45:
                        # check positions 7-14 are hex (0-9 or A-F)
                        c8 = ord(row[8])
                        c9 = ord(row[9])
                        c10 = ord(row[10])
                        c11 = ord(row[11])
                        c12 = ord(row[12])
                        c13 = ord(row[13])
                        c14 = ord(row[14])
                        # check hex: (48 <= c <= 57) or (65 <= c <= 70)
                        if (48 <= c7 <= 57 or 65 <= c7 <= 70) and \
                           (48 <= c8 <= 57 or 65 <= c8 <= 70) and \
                           (48 <= c9 <= 57 or 65 <= c9 <= 70) and \
                           (48 <= c10 <= 57 or 65 <= c10 <= 70) and \
                           (48 <= c11 <= 57 or 65 <= c11 <= 70) and \
                           (48 <= c12 <= 57 or 65 <= c12 <= 70) and \
                           (48 <= c13 <= 57 or 65 <= c13 <= 70) and \
                           (48 <= c14 <= 57 or 65 <= c14 <= 70):
                            v = c2 + c7
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h