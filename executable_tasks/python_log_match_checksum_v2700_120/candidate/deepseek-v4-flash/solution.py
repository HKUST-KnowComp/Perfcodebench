def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211
    for row in rows:
        v = 0
        if len(row) == 15:
            c0 = row[0]
            c1 = row[1]
            if 'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z':
                d2 = row[2]
                d3 = row[3]
                d4 = row[4]
                d5 = row[5]
                if ('0' <= d2 <= '9' and '0' <= d3 <= '9' and
                    '0' <= d4 <= '9' and '0' <= d5 <= '9'):
                    if row[6] == '-':
                        c7 = row[7]
                        c8 = row[8]
                        c9 = row[9]
                        c10 = row[10]
                        c11 = row[11]
                        c12 = row[12]
                        c13 = row[13]
                        c14 = row[14]
                        if (('0' <= c7 <= '9') or ('A' <= c7 <= 'F')) and \
                           (('0' <= c8 <= '9') or ('A' <= c8 <= 'F')) and \
                           (('0' <= c9 <= '9') or ('A' <= c9 <= 'F')) and \
                           (('0' <= c10 <= '9') or ('A' <= c10 <= 'F')) and \
                           (('0' <= c11 <= '9') or ('A' <= c11 <= 'F')) and \
                           (('0' <= c12 <= '9') or ('A' <= c12 <= 'F')) and \
                           (('0' <= c13 <= '9') or ('A' <= c13 <= 'F')) and \
                           (('0' <= c14 <= '9') or ('A' <= c14 <= 'F')):
                            v = ord(d2) + ord(c7)
        h ^= v + 1
        h *= mul
        h &= mask
    return h
