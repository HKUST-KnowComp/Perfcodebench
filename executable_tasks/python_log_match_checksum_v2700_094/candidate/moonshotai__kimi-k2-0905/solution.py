import sys


PAT = sys.modules['builtins'].__dict__.get('PAT', None)   # keep original import unused to avoid changing the header signature

def run(rows: list[str]) -> int:
    """
    Same logic as the baseline, only fixed-pattern matching is done inline without regex.
    """
    h = 1469598103934665603
    for row in rows:
        # fast path rejection: wrong length -> skip
        if len(row) != 16:
            v = 0
        else:
            # Check first two chars are ASCII 65-90 (A-Z)
            a1 = (row[0] | 32) - 97          # maps A-Z to 0-25
            a2 = (row[1] | 32) - 97
            if a1 < 0 or a1 > 25 or a2 < 0 or a2 > 25:
                v = 0
            # chars 2-5 must be ASCII 48-57 (0-9)
            elif not (48 <= ord(row[2]) <= 57 and
                      48 <= ord(row[3]) <= 57 and
                      48 <= ord(row[4]) <= 57 and
                      48 <= ord(row[5]) <= 57):
                v = 0
            # char 6 is hyphen
            elif row[6] != 45:
                v = 0
            # chars 7-14 must be ASCII 48-57 (0-9) or 65-70/97-102 (A-F, a-f)
            else:
                valid = True
                for i in range(7, 15):
                    c = ord(row[i])
                    if not (48 <= c <= 57 or 65 <= c <= 70 or 97 <= c <= 102):
                        valid = False
                        break
                v = (ord(row[2]) + ord(row[7])) if valid else 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h