import sys

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    for row in rows:
        if len(row) != 14:
            v = 0
        else:
            # positions 0,1 must be uppercase letters
            c0 = row[0]
            c1 = row[1]
            if c0 < 'A' or c0 > 'Z' or c1 < 'A' or c1 > 'Z':
                v = 0
            else:
                # positions 2,3,4,5 must be digits
                c2 = row[2]
                c3 = row[3]
                c4 = row[4]
                c5 = row[5]
                if c2 < '0' or c2 > '9' or c3 < '0' or c3 > '9' or c4 < '0' or c4 > '9' or c5 < '0' or c5 > '9':
                    v = 0
                else:
                    # position 6 must be hyphen
                    if row[6] != '-':
                        v = 0
                    else:
                        # positions 7..14 must be hex digits
                        valid = True
                        for i in range(7, 15):
                            ch = row[i]
                            if not (('0' <= ch <= '9') or ('A' <= ch <= 'F')):
                                valid = False
                                break
                        if valid:
                            v = ord(row[2]) + ord(row[7])
                        else:
                            v = 0
        h ^= v + 1
        h = (h * 1099511628211) & mask64
    return h