import re


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211
    _len = len
    _ord = ord

    for row in rows:
        valid = False
        # manual length & character pattern validation
        if _len(row) == 15:
            c0 = _ord(row[0])
            c1 = _ord(row[1])
            if 65 <= c0 <= 90 and 65 <= c1 <= 90:  # 'A'..'Z'
                d2 = _ord(row[2])
                d3 = _ord(row[3])
                d4 = _ord(row[4])
                d5 = _ord(row[5])
                if (48 <= d2 <= 57 and 48 <= d3 <= 57 and
                    48 <= d4 <= 57 and 48 <= d5 <= 57):  # '0'..'9'
                    if _ord(row[6]) == 45:  # '-'
                        c7 = _ord(row[7])
                        c8 = _ord(row[8])
                        c9 = _ord(row[9])
                        c10 = _ord(row[10])
                        c11 = _ord(row[11])
                        c12 = _ord(row[12])
                        c13 = _ord(row[13])
                        c14 = _ord(row[14])
                        # hex digits: '0'..'9' or 'A'..'F'
                        if ((48 <= c7 <= 57 or 65 <= c7 <= 70) and
                            (48 <= c8 <= 57 or 65 <= c8 <= 70) and
                            (48 <= c9 <= 57 or 65 <= c9 <= 70) and
                            (48 <= c10 <= 57 or 65 <= c10 <= 70) and
                            (48 <= c11 <= 57 or 65 <= c11 <= 70) and
                            (48 <= c12 <= 57 or 65 <= c12 <= 70) and
                            (48 <= c13 <= 57 or 65 <= c13 <= 70) and
                            (48 <= c14 <= 57 or 65 <= c14 <= 70)):
                            valid = True

        if valid:
            v = d2 + c7  # ord(row[2]) + ord(row[7])
        else:
            v = 0

        h = ((h ^ (v + 1)) * mul) & mask

    return h