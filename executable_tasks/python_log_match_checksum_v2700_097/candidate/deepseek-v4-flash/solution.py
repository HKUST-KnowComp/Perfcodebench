def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    mul = 1099511628211
    for row in rows:
        v = 0
        if len(row) == 15:
            c0 = row[0]
            if 'A' <= c0 <= 'Z':
                c1 = row[1]
                if 'A' <= c1 <= 'Z':
                    c2 = row[2]
                    if '0' <= c2 <= '9':
                        c3 = row[3]
                        if '0' <= c3 <= '9':
                            c4 = row[4]
                            if '0' <= c4 <= '9':
                                c5 = row[5]
                                if '0' <= c5 <= '9':
                                    if row[6] == '-':
                                        c7 = row[7]
                                        if ('0' <= c7 <= '9') or ('A' <= c7 <= 'F'):
                                            c8 = row[8]
                                            if ('0' <= c8 <= '9') or ('A' <= c8 <= 'F'):
                                                c9 = row[9]
                                                if ('0' <= c9 <= '9') or ('A' <= c9 <= 'F'):
                                                    c10 = row[10]
                                                    if ('0' <= c10 <= '9') or ('A' <= c10 <= 'F'):
                                                        c11 = row[11]
                                                        if ('0' <= c11 <= '9') or ('A' <= c11 <= 'F'):
                                                            c12 = row[12]
                                                            if ('0' <= c12 <= '9') or ('A' <= c12 <= 'F'):
                                                                c13 = row[13]
                                                                if ('0' <= c13 <= '9') or ('A' <= c13 <= 'F'):
                                                                    c14 = row[14]
                                                                    if ('0' <= c14 <= '9') or ('A' <= c14 <= 'F'):
                                                                        v = ord(c2) + ord(c7)
        h ^= v + 1
        h *= mul
        h &= mask64
    return h