def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    ord_A = 65
    ord_Z = 90
    ord_0 = 48
    ord_9 = 57
    ord_dash = 45
    ord_a = 97
    ord_f = 102
    for row in rows:
        if len(row) == 15:
            c0 = ord(row[0])
            c1 = ord(row[1])
            c2 = ord(row[2])
            c3 = ord(row[3])
            c4 = ord(row[4])
            c5 = ord(row[5])
            c6 = ord(row[6])
            c7 = ord(row[7])
            c8 = ord(row[8])
            c9 = ord(row[9])
            c10 = ord(row[10])
            c11 = ord(row[11])
            c12 = ord(row[12])
            c13 = ord(row[13])
            c14 = ord(row[14])
            if (ord_A <= c0 <= ord_Z and
                ord_A <= c1 <= ord_Z and
                ord_0 <= c2 <= ord_9 and
                ord_0 <= c3 <= ord_9 and
                ord_0 <= c4 <= ord_9 and
                ord_0 <= c5 <= ord_9 and
                c6 == ord_dash and
                (ord_0 <= c7 <= ord_9 or ord_A <= c7 <= ord_Z) and
                (ord_0 <= c8 <= ord_9 or ord_A <= c8 <= ord_Z) and
                (ord_0 <= c9 <= ord_9 or ord_A <= c9 <= ord_Z) and
                (ord_0 <= c10 <= ord_9 or ord_A <= c10 <= ord_Z) and
                (ord_0 <= c11 <= ord_9 or ord_A <= c11 <= ord_Z) and
                (ord_0 <= c12 <= ord_9 or ord_A <= c12 <= ord_Z) and
                (ord_0 <= c13 <= ord_9 or ord_A <= c13 <= ord_Z) and
                (ord_0 <= c14 <= ord_9 or ord_A <= c14 <= ord_Z)):
                v = c2 + c7
            else:
                v = 0
        else:
            v = 0
        h ^= v + 1
        h *= mult
        h &= mask
    return h
