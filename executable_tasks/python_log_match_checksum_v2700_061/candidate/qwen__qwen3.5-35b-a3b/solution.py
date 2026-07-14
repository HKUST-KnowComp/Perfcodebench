def run(rows: list[str]) -> int:
    h = 1469598103934665603
    hex_set = set('0123456789ABCDEF')
    for row in rows:
        if len(row) != 15:
            continue
        if not ('A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z'):
            continue
        if not row[2:6].isdigit():
            continue
        if row[6] != '-':
            continue
        hex_part = row[7:15]
        if not all(c in hex_set for c in hex_part):
            continue
        v = ord(row[2]) + ord(row[7])
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h