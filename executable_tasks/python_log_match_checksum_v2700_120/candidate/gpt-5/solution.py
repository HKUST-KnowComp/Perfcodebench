def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    ord_ = ord

    for s in rows:
        v = 0
        if len(s) == 15:
            o0 = ord_(s[0])
            o1 = ord_(s[1])
            # A-Z for first two
            if 65 <= o0 <= 90 and 65 <= o1 <= 90:
                o2 = ord_(s[2])
                o3 = ord_(s[3])
                o4 = ord_(s[4])
                o5 = ord_(s[5])
                # 0-9 for next four and '-' at pos 6
                if 48 <= o2 <= 57 and 48 <= o3 <= 57 and 48 <= o4 <= 57 and 48 <= o5 <= 57 and s[6] == '-':
                    o7 = ord_(s[7])
                    o8 = ord_(s[8])
                    o9 = ord_(s[9])
                    o10 = ord_(s[10])
                    o11 = ord_(s[11])
                    o12 = ord_(s[12])
                    o13 = ord_(s[13])
                    o14 = ord_(s[14])
                    # Hex uppercase 0-9 or A-F for positions 7..14
                    if (((48 <= o7 <= 57) or (65 <= o7 <= 70)) and
                        ((48 <= o8 <= 57) or (65 <= o8 <= 70)) and
                        ((48 <= o9 <= 57) or (65 <= o9 <= 70)) and
                        ((48 <= o10 <= 57) or (65 <= o10 <= 70)) and
                        ((48 <= o11 <= 57) or (65 <= o11 <= 70)) and
                        ((48 <= o12 <= 57) or (65 <= o12 <= 70)) and
                        ((48 <= o13 <= 57) or (65 <= o13 <= 70)) and
                        ((48 <= o14 <= 57) or (65 <= o14 <= 70))):
                        v = o2 + o7
        h ^= v + 1
        h = (h * FNV_PRIME) & MASK
    return h
