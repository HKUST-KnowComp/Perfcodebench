UPPERCASE_LETTERS = set('ABCDEFGHIJKLMNOPQRSTUVWXYZ')
DIGITS = set('0123456789')
HEX_CHARS = set('0123456789ABCDEF')

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if len(row) != 15:
            continue
        if row[0] not in UPPERCASE_LETTERS or row[1] not in UPPERCASE_LETTERS:
            continue
        if not (row[2] in DIGITS and row[3] in DIGITS and row[4] in DIGITS and row[5] in DIGITS):
            continue
        if row[6] != '-':
            continue
        if not all(c in HEX_CHARS for c in row[7:15]):
            continue
        v = ord(row[2]) + ord(row[7])
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h