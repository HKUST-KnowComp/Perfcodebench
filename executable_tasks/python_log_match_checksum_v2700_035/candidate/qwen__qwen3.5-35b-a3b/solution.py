VALID_HEX = set('0123456789ABCDEF')

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if len(row) != 15:
            v = 0
        else:
            if not ('A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z'):
                v = 0
            elif not row[2:6].isdigit():
                v = 0
            elif row[6] != '-':
                v = 0
            elif not all(c in VALID_HEX for c in row[7:15]):
                v = 0
            else:
                v = ord(row[2]) + ord(row[7])
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h