def run(rows: list[str]) -> int:
    h = 1469598103934665603
    HEX = frozenset('0123456789ABCDEF')
    
    for row in rows:
        if (len(row) == 15 and
            row[2].isdigit() and
            row[3].isdigit() and
            row[4].isdigit() and
            row[5].isdigit() and
            row[6] == '-' and
            'A' <= row[0] <= 'Z' and
            'A' <= row[1] <= 'Z' and
            row[7] in HEX and
            row[8] in HEX and
            row[9] in HEX and
            row[10] in HEX and
            row[11] in HEX and
            row[12] in HEX and
            row[13] in HEX and
            row[14] in HEX):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
