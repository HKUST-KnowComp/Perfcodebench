def run(rows: list[str]) -> int:
    h = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    for row in rows:
        v = 0
        if len(row) == 15:
            if 'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z':
                if '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9':
                    if row[6] == '-':
                        c7 = row[7]; c8 = row[8]; c9 = row[9]; c10 = row[10]; c11 = row[11]; c12 = row[12]; c13 = row[13]; c14 = row[14]
                        if (('0' <= c7 <= '9' or 'A' <= c7 <= 'F') and
                            ('0' <= c8 <= '9' or 'A' <= c8 <= 'F') and
                            ('0' <= c9 <= '9' or 'A' <= c9 <= 'F') and
                            ('0' <= c10 <= '9' or 'A' <= c10 <= 'F') and
                            ('0' <= c11 <= '9' or 'A' <= c11 <= 'F') and
                            ('0' <= c12 <= '9' or 'A' <= c12 <= 'F') and
                            ('0' <= c13 <= '9' or 'A' <= c13 <= 'F') and
                            ('0' <= c14 <= '9' or 'A' <= c14 <= 'F')):
                            v = ord(row[2]) + ord(c7)
        h ^= v + 1
        h *= multiplier
        h &= mask
    return h
