import re


PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Pattern: [A-Z]{2}[0-9]{4}-[0-9A-F]{8}
        # Total length must be 15
        if len(row) == 15 and row[6] == '-':
            # Check [A-Z]{2}
            if 65 <= ord(row[0]) <= 90 and 65 <= ord(row[1]) <= 90:
                # Check [0-9]{4}
                if (48 <= ord(row[2]) <= 57 and 48 <= ord(row[3]) <= 57 and
                    48 <= ord(row[4]) <= 57 and 48 <= ord(row[5]) <= 57):
                    # Check [0-9A-F]{8}
                    valid = True
                    for i in range(7, 15):
                        c = ord(row[i])
                        if not ((48 <= c <= 57) or (65 <= c <= 70)):
                            valid = False
                            break
                    if valid:
                        v = ord(row[2]) + ord(row[7])
                    else:
                        v = 0
                else:
                    v = 0
            else:
                v = 0
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
