def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mul = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    for row in rows:
        if len(row) != 15:
            v = 0
        else:
            c0 = row[0]
            c1 = row[1]
            c2 = row[2]
            c3 = row[3]
            c4 = row[4]
            c5 = row[5]
            c6 = row[6]
            c7 = row[7]
            c8 = row[8]
            c9 = row[9]
            c10 = row[10]
            c11 = row[11]
            c12 = row[12]
            c13 = row[13]
            c14 = row[14]
            if (
                'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z' and
                '0' <= c2 <= '9' and '0' <= c3 <= '9' and '0' <= c4 <= '9' and '0' <= c5 <= '9' and
                c6 == '-' and
                ('0' <= c7 <= '9' or 'A' <= c7 <= 'F') and
                ('0' <= c8 <= '9' or 'A' <= c8 <= 'F') and
                ('0' <= c9 <= '9' or 'A' <= c9 <= 'F') and
                ('0' <= c10 <= '9' or 'A' <= c10 <= 'F') and
                ('0' <= c11 <= '9' or 'A' <= c11 <= 'F') and
                ('0' <= c12 <= '9' or 'A' <= c12 <= 'F') and
                ('0' <= c13 <= '9' or 'A' <= c13 <= 'F') and
                ('0' <= c14 <= '9' or 'A' <= c14 <= 'F')
            ):
                v = ord(c2) + ord(c7)
            else:
                v = 0
        h ^= v + 1
        h *= mul
        h &= mask
    return h
