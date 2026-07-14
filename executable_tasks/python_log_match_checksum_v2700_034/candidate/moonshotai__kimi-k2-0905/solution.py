import sys

def run(rows: list[str]) -> int:
    h = 14695981039346656037 & 0xFFFFFFFFFFFFFFFF
    for row in rows:
        if len(row) != 14:
            v = 0
        else:
            # positions 0,1 must be A-Z
            c0, c1 = row[0], row[1]
            if (c0 < 'A' or c0 > 'Z') or (c1 < 'A' or c1 > 'Z'):
                v = 0
            else:
                # positions 2,3,4,5 must be 0-9
                c2, c3, c4, c5 = row[2], row[3], row[4], row[5]
                if ((c2 < '0' or c2 > '9') or
                    (c3 < '0' or c3 > '9') or
                    (c4 < '0' or c4 > '9') or
                    (c5 < '0' or c5 > '9')):
                    v = 0
                else:
                    # position 6 must be '-'
                    if row[6] != '-':
                        v = 0
                    else:
                        # positions 7..14 must be 0-9 or A-F
                        tail = row[7:15]
                        valid = True
                        for ch in tail:
                            if not ('0' <= ch <= '9' or 'A' <= ch <= 'F'):
                                valid = False
                                break
                        if not valid:
                            v = 0
                        else:
                            v = (ord(row[2]) + ord(row[7]))
        h ^= (v + 1)
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h