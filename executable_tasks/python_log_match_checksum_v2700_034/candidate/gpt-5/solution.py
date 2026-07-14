def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    mul = 1099511628211
    mask = (1 << 64) - 1

    loc_ord = ord

    for row in rows:
        v = 0
        if len(row) == 15:
            c0 = row[0]
            c1 = row[1]
            c2 = row[2]
            c3 = row[3]
            c4 = row[4]
            c5 = row[5]
            c6 = row[6]
            # Check two uppercase letters, four digits, and hyphen
            if ('A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z' and
                '0' <= c2 <= '9' and '0' <= c3 <= '9' and
                '0' <= c4 <= '9' and '0' <= c5 <= '9' and
                c6 == '-'):
                c7 = row[7]
                c8 = row[8]
                c9 = row[9]
                ca = row[10]
                cb = row[11]
                cc = row[12]
                cd = row[13]
                ce = row[14]
                # Check eight uppercase hex characters
                if (((('0' <= c7 <= '9') or ('A' <= c7 <= 'F')) and
                     (('0' <= c8 <= '9') or ('A' <= c8 <= 'F')) and
                     (('0' <= c9 <= '9') or ('A' <= c9 <= 'F')) and
                     (('0' <= ca <= '9') or ('A' <= ca <= 'F')) and
                     (('0' <= cb <= '9') or ('A' <= cb <= 'F')) and
                     (('0' <= cc <= '9') or ('A' <= cc <= 'F')) and
                     (('0' <= cd <= '9') or ('A' <= cd <= 'F')) and
                     (('0' <= ce <= '9') or ('A' <= ce <= 'F')))):
                    v = loc_ord(c2) + loc_ord(c7)
        h ^= v + 1
        h = (h * mul) & mask

    return h
