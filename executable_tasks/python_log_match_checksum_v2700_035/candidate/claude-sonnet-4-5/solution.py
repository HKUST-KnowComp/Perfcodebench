def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if (len(row) == 15 and
            65 <= ord(row[0]) <= 90 and
            65 <= ord(row[1]) <= 90 and
            48 <= ord(row[2]) <= 57 and
            48 <= ord(row[3]) <= 57 and
            48 <= ord(row[4]) <= 57 and
            48 <= ord(row[5]) <= 57 and
            row[6] == '-' and
            ((48 <= ord(row[7]) <= 57) or (65 <= ord(row[7]) <= 70)) and
            ((48 <= ord(row[8]) <= 57) or (65 <= ord(row[8]) <= 70)) and
            ((48 <= ord(row[9]) <= 57) or (65 <= ord(row[9]) <= 70)) and
            ((48 <= ord(row[10]) <= 57) or (65 <= ord(row[10]) <= 70)) and
            ((48 <= ord(row[11]) <= 57) or (65 <= ord(row[11]) <= 70)) and
            ((48 <= ord(row[12]) <= 57) or (65 <= ord(row[12]) <= 70)) and
            ((48 <= ord(row[13]) <= 57) or (65 <= ord(row[13]) <= 70)) and
            ((48 <= ord(row[14]) <= 57) or (65 <= ord(row[14]) <= 70))):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
