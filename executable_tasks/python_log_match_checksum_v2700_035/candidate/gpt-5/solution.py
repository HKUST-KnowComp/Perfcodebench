def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = 18446744073709551615  # (1<<64)-1

    ord_ = ord
    o0 = 48
    o9 = 57
    oA = 65
    oF = 70
    oZ = 90

    for row in rows:
        v = 0
        if len(row) == 15:
            c0 = ord_(row[0])
            c1 = ord_(row[1])
            # First two uppercase letters
            if oA <= c0 <= oZ and oA <= c1 <= oZ:
                c2 = ord_(row[2])
                c3 = ord_(row[3])
                c4 = ord_(row[4])
                c5 = ord_(row[5])
                # Next four digits and a hyphen
                if (o0 <= c2 <= o9 and o0 <= c3 <= o9 and o0 <= c4 <= o9 and o0 <= c5 <= o9 and row[6] == '-'):
                    # Next eight hex uppercase digits (0-9, A-F)
                    c7 = ord_(row[7])
                    if (o0 <= c7 <= o9) or (oA <= c7 <= oF):
                        c8 = ord_(row[8])
                        if ((o0 <= c8 <= o9) or (oA <= c8 <= oF)):
                            c9 = ord_(row[9])
                            if ((o0 <= c9 <= o9) or (oA <= c9 <= oF)):
                                c10 = ord_(row[10])
                                if ((o0 <= c10 <= o9) or (oA <= c10 <= oF)):
                                    c11 = ord_(row[11])
                                    if ((o0 <= c11 <= o9) or (oA <= c11 <= oF)):
                                        c12 = ord_(row[12])
                                        if ((o0 <= c12 <= o9) or (oA <= c12 <= oF)):
                                            c13 = ord_(row[13])
                                            if ((o0 <= c13 <= o9) or (oA <= c13 <= oF)):
                                                c14 = ord_(row[14])
                                                if ((o0 <= c14 <= o9) or (oA <= c14 <= oF)):
                                                    v = c2 + c7
        # FNV-1a mix
        h ^= (v + 1)
        h *= prime
        h &= mask

    return h
