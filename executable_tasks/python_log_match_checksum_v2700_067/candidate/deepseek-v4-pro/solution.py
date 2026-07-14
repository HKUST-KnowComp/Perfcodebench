def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mult = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    _ord = ord
    for row in rows:
        valid = False
        if len(row) == 15:
            if 'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z':
                if '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9':
                    if row[6] == '-':
                        # Unrolled hex-digit checks for indices 7-14
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
                                                        valid = True
        if valid:
            v = _ord(row[2]) + _ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= mult
        h &= mask
    return h
