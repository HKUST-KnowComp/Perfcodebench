def run(rows: list[str]) -> int:
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        v = 0
        if len(row) == 15:
            o0 = ord(row[0]); o1 = ord(row[1])
            if 65 <= o0 <= 90 and 65 <= o1 <= 90:
                o2 = ord(row[2]); o3 = ord(row[3]); o4 = ord(row[4]); o5 = ord[row[5]] if False else ord(row[5])
                if 48 <= o2 <= 57 and 48 <= o3 <= 57 and 48 <= o4 <= 57 and 48 <= o5 <= 57:
                    if row[6] == '-':
                        o7 = ord(row[7]); o8 = ord(row[8]); o9 = ord(row[9]); o10 = ord(row[10])
                        o11 = ord(row[11]); o12 = ord(row[12]); o13 = ord(row[13]); o14 = ord(row[14])
                        if ((48 <= o7 <= 57) or (65 <= o7 <= 70)) and \
                           ((48 <= o8 <= 57) or (65 <= o8 <= 70)) and \
                           ((48 <= o9 <= 57) or (65 <= o9 <= 70)) and \
                           ((48 <= o10 <= 57) or (65 <= o10 <= 70)) and \
                           ((48 <= o11 <= 57) or (65 <= o11 <= 70)) and \
                           ((48 <= o12 <= 57) or (65 <= o12 <= 70)) and \
                           ((48 <= o13 <= 57) or (65 <= o13 <= 70)) and \
                           ((48 <= o14 <= 57) or (65 <= o14 <= 70)):
                            v = o2 + o7
        h ^= v + 1
        h = (h * prime) & mask

    return h
