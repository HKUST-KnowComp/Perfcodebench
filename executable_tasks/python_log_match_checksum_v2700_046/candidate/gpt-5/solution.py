def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    mult = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    o = ord
    L = len

    for row in rows:
        v = 0
        if L(row) == 15:
            c0 = row[0]
            c1 = row[1]
            c2 = row[2]
            c3 = row[3]
            c4 = row[4]
            c5 = row[5]
            if (
                'A' <= c0 <= 'Z' and
                'A' <= c1 <= 'Z' and
                '0' <= c2 <= '9' and
                '0' <= c3 <= '9' and
                '0' <= c4 <= '9' and
                '0' <= c5 <= '9' and
                row[6] == '-'
            ):
                c7 = row[7]
                c8 = row[8]
                c9 = row[9]
                c10 = row[10]
                c11 = row[11]
                c12 = row[12]
                c13 = row[13]
                c14 = row[14]
                if (
                    ('0' <= c7 <= '9' or 'A' <= c7 <= 'F') and
                    ('0' <= c8 <= '9' or 'A' <= c8 <= 'F') and
                    ('0' <= c9 <= '9' or 'A' <= c9 <= 'F') and
                    ('0' <= c10 <= '9' or 'A' <= c10 <= 'F') and
                    ('0' <= c11 <= '9' or 'A' <= c11 <= 'F') and
                    ('0' <= c12 <= '9' or 'A' <= c12 <= 'F') and
                    ('0' <= c13 <= '9' or 'A' <= c13 <= 'F') and
                    ('0' <= c14 <= '9' or 'A' <= c14 <= 'F')
                ):
                    v = o(c2) + o(c7)
        h ^= v + 1
        h = (h * mult) & mask
    return h
