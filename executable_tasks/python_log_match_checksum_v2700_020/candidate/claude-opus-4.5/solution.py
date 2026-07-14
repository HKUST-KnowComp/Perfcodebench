def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    ord_ = ord
    for row in rows:
        # Check length first (must be exactly 15)
        if len(row) == 15:
            # Check format: AA9999-HHHHHHHH
            # row[0], row[1]: A-Z
            # row[2..5]: 0-9
            # row[6]: -
            # row[7..14]: 0-9 or A-F
            c0 = ord_(row[0])
            c1 = ord_(row[1])
            c2 = ord_(row[2])
            c3 = ord_(row[3])
            c4 = ord_(row[4])
            c5 = ord_(row[5])
            c6 = ord_(row[6])
            c7 = ord_(row[7])
            c8 = ord_(row[8])
            c9 = ord_(row[9])
            c10 = ord_(row[10])
            c11 = ord_(row[11])
            c12 = ord_(row[12])
            c13 = ord_(row[13])
            c14 = ord_(row[14])
            # A-Z: 65-90, 0-9: 48-57, dash: 45, A-F: 65-70
            if (65 <= c0 <= 90 and
                65 <= c1 <= 90 and
                48 <= c2 <= 57 and
                48 <= c3 <= 57 and
                48 <= c4 <= 57 and
                48 <= c5 <= 57 and
                c6 == 45 and
                (48 <= c7 <= 57 or 65 <= c7 <= 70) and
                (48 <= c8 <= 57 or 65 <= c8 <= 70) and
                (48 <= c9 <= 57 or 65 <= c9 <= 70) and
                (48 <= c10 <= 57 or 65 <= c10 <= 70) and
                (48 <= c11 <= 57 or 65 <= c11 <= 70) and
                (48 <= c12 <= 57 or 65 <= c12 <= 70) and
                (48 <= c13 <= 57 or 65 <= c13 <= 70) and
                (48 <= c14 <= 57 or 65 <= c14 <= 70)):
                v = c2 + c7
            else:
                v = 0
        else:
            v = 0
        h ^= v + 1
        h = (h * mult) & mask
    return h
