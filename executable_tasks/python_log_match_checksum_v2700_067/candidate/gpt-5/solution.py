OFFSET = 1469598103934665603
PRIME = 1099511628211
MASK = 0xFFFFFFFFFFFFFFFF


def run(rows: list[str]) -> int:
    h = OFFSET
    prime = PRIME
    mask = MASK
    ord_ = ord
    for row in rows:
        v = 0
        if len(row) == 15 and row[6] == '-':
            c0 = row[0]
            c1 = row[1]
            # First two uppercase letters A-Z
            if 'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z':
                d0 = row[2]
                d1 = row[3]
                d2 = row[4]
                d3 = row[5]
                # Four digits 0-9
                if ('0' <= d0 <= '9' and '0' <= d1 <= '9' and '0' <= d2 <= '9' and '0' <= d3 <= '9'):
                    b0 = row[7]
                    b1 = row[8]
                    b2 = row[9]
                    b3 = row[10]
                    b4 = row[11]
                    b5 = row[12]
                    b6 = row[13]
                    b7 = row[14]
                    # Eight hex uppercase digits 0-9 or A-F
                    if (('0' <= b0 <= '9' or 'A' <= b0 <= 'F') and
                        ('0' <= b1 <= '9' or 'A' <= b1 <= 'F') and
                        ('0' <= b2 <= '9' or 'A' <= b2 <= 'F') and
                        ('0' <= b3 <= '9' or 'A' <= b3 <= 'F') and
                        ('0' <= b4 <= '9' or 'A' <= b4 <= 'F') and
                        ('0' <= b5 <= '9' or 'A' <= b5 <= 'F') and
                        ('0' <= b6 <= '9' or 'A' <= b6 <= 'F') and
                        ('0' <= b7 <= '9' or 'A' <= b7 <= 'F')):
                        v = ord_(d0) + ord_(b0)
        h ^= v + 1
        h *= prime
        h &= mask
    return h
