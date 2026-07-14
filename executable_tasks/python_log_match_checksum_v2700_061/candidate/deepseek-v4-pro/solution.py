def fast_is_match(s: str) -> bool:
    if len(s) != 15:
        return False
    c0, c1 = s[0], s[1]
    if not (('A' <= c0 <= 'Z') and ('A' <= c1 <= 'Z')):
        return False
    # digits at positions 2,3,4,5
    if not (('0' <= s[2] <= '9') and ('0' <= s[3] <= '9') and ('0' <= s[4] <= '9') and ('0' <= s[5] <= '9')):
        return False
    if s[6] != '-':
        return False
    # hex at positions 7..14
    for i in range(7, 15):
        c = s[i]
        if not (('0' <= c <= '9') or ('A' <= c <= 'F')):
            return False
    return True

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MUL = 1099511628211
    MASK = (1 << 64) - 1
    for row in rows:
        if fast_is_match(row):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h = (h * MUL) & MASK
    return h
