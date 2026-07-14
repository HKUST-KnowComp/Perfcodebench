def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    for row in rows:
        v = 0
        if len(row) == 15:
            c0 = row[0]
            c1 = row[1]
            if ('A' <= c0 <= 'Z') and ('A' <= c1 <= 'Z'):
                d2 = row[2]
                d3 = row[3]
                d4 = row[4]
                d5 = row[5]
                if ('0' <= d2 <= '9') and ('0' <= d3 <= '9') and ('0' <= d4 <= '9') and ('0' <= d5 <= '9'):
                    if row[6] == '-':
                        h7 = row[7]
                        h8 = row[8]
                        h9 = row[9]
                        h10 = row[10]
                        h11 = row[11]
                        h12 = row[12]
                        h13 = row[13]
                        h14 = row[14]
                        if (('0' <= h7 <= '9') or ('A' <= h7 <= 'F')) and \
                           (('0' <= h8 <= '9') or ('A' <= h8 <= 'F')) and \
                           (('0' <= h9 <= '9') or ('A' <= h9 <= 'F')) and \
                           (('0' <= h10 <= '9') or ('A' <= h10 <= 'F')) and \
                           (('0' <= h11 <= '9') or ('A' <= h11 <= 'F')) and \
                           (('0' <= h12 <= '9') or ('A' <= h12 <= 'F')) and \
                           (('0' <= h13 <= '9') or ('A' <= h13 <= 'F')) and \
                           (('0' <= h14 <= '9') or ('A' <= h14 <= 'F')):
                            v = ord(row[2]) + ord(row[7])
        h ^= v + 1
        h *= mult
        h &= mask
    return h