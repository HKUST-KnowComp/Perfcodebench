def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    hex_chars = frozenset(b'0123456789ABCDEF')
    for row in rows:
        if len(row) == 15:
            c0 = row[0]
            c1 = row[1]
            c2 = row[2]
            c3 = row[3]
            c4 = row[4]
            c5 = row[5]
            c6 = row[6]
            c7 = row[7]
            if ('A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z' and
                '0' <= c2 <= '9' and '0' <= c3 <= '9' and
                '0' <= c4 <= '9' and '0' <= c5 <= '9' and
                c6 == '-'):
                r = row.encode('latin-1')
                if (r[7] in hex_chars and r[8] in hex_chars and
                    r[9] in hex_chars and r[10] in hex_chars and
                    r[11] in hex_chars and r[12] in hex_chars and
                    r[13] in hex_chars and r[14] in hex_chars):
                    v = ord(c2) + ord(c7)
                else:
                    v = 0
            else:
                v = 0
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
