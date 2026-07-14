def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mul = 1099511628211
    A_lo = ord('A')
    A_hi = ord('Z')
    zero_lo = ord('0')
    zero_hi = ord('9')
    F_hi = ord('F')

    for row in rows:
        if len(row) == 15:
            c0 = ord(row[0])
            c1 = ord(row[1])
            if A_lo <= c0 <= A_hi and A_lo <= c1 <= A_hi:
                c2 = ord(row[2])
                c3 = ord(row[3])
                c4 = ord(row[4])
                c5 = ord(row[5])
                if zero_lo <= c2 <= zero_hi and zero_lo <= c3 <= zero_hi and zero_lo <= c4 <= zero_hi and zero_lo <= c5 <= zero_hi:
                    if row[6] == '-':
                        c7 = ord(row[7])
                        if (zero_lo <= c7 <= zero_hi) or (A_lo <= c7 <= F_hi):
                            c8 = ord(row[8])
                            if (zero_lo <= c8 <= zero_hi) or (A_lo <= c8 <= F_hi):
                                c9 = ord(row[9])
                                if (zero_lo <= c9 <= zero_hi) or (A_lo <= c9 <= F_hi):
                                    c10 = ord(row[10])
                                    if (zero_lo <= c10 <= zero_hi) or (A_lo <= c10 <= F_hi):
                                        c11 = ord(row[11])
                                        if (zero_lo <= c11 <= zero_hi) or (A_lo <= c11 <= F_hi):
                                            c12 = ord(row[12])
                                            if (zero_lo <= c12 <= zero_hi) or (A_lo <= c12 <= F_hi):
                                                c13 = ord(row[13])
                                                if (zero_lo <= c13 <= zero_hi) or (A_lo <= c13 <= F_hi):
                                                    c14 = ord(row[14])
                                                    if (zero_lo <= c14 <= zero_hi) or (A_lo <= c14 <= F_hi):
                                                        v = c2 + c7
                                                    else:
                                                        v = 0
                                                else:
                                                    v = 0
                                            else:
                                                v = 0
                                        else:
                                            v = 0
                                    else:
                                        v = 0
                                else:
                                    v = 0
                            else:
                                v = 0
                        else:
                            v = 0
                    else:
                        v = 0
                else:
                    v = 0
            else:
                v = 0
        else:
            v = 0

        h = ((h ^ (v + 1)) * mul) & mask

    return h
