def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mult = 1099511628211
    for row in rows:
        if len(row) == 15:
            c0 = row[0]
            if 'A' <= c0 <= 'Z':
                c1 = row[1]
                if 'A' <= c1 <= 'Z':
                    if '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9':
                        if row[6] == '-':
                            c7 = row[7]
                            if ('0' <= c7 <= '9' or 'A' <= c7 <= 'F') and \
                               ('0' <= row[8] <= '9' or 'A' <= row[8] <= 'F') and \
                               ('0' <= row[9] <= '9' or 'A' <= row[9] <= 'F') and \
                               ('0' <= row[10] <= '9' or 'A' <= row[10] <= 'F') and \
                               ('0' <= row[11] <= '9' or 'A' <= row[11] <= 'F') and \
                               ('0' <= row[12] <= '9' or 'A' <= row[12] <= 'F') and \
                               ('0' <= row[13] <= '9' or 'A' <= row[13] <= 'F') and \
                               ('0' <= row[14] <= '9' or 'A' <= row[14] <= 'F'):
                                v = ord(row[2]) + ord(c7)
                                h ^= v + 1
                                h = (h * mult) & mask
                                continue
        h ^= 1
        h = (h * mult) & mask
    return h
