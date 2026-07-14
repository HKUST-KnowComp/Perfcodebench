def run(rows: list[str]) -> int:
    hex_chars = set('0123456789ABCDEF')
    h = 1469598103934665603
    for row in rows:
        if len(row) != 15:
            continue
        if not ('A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z'):
            continue
        if not ('0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9'):
            continue
        if row[6] != '-':
            continue
        if not all(c in hex_chars for c in row[7:15]):
            continue
        v = ord(row[2]) + ord(row[7])
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h