def run(rows: list[str]) -> int:
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    o = ord

    for s in rows:
        v = 0
        if len(s) == 15:
            c0 = s[0]
            c1 = s[1]
            if 'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z':
                c2 = s[2]
                c3 = s[3]
                c4 = s[4]
                c5 = s[5]
                if '0' <= c2 <= '9' and '0' <= c3 <= '9' and '0' <= c4 <= '9' and '0' <= c5 <= '9' and s[6] == '-':
                    c7 = s[7]
                    c8 = s[8]
                    c9 = s[9]
                    c10 = s[10]
                    c11 = s[11]
                    c12 = s[12]
                    c13 = s[13]
                    c14 = s[14]
                    if (
                        ('0' <= c7 <= '9' or 'A' <= c7 <= 'F') and
                        ('0' <= c8 <= '9' or 'A' <= c8 <= 'F') and
                        ('0' <= c9 <= '9' or 'A' <= c9 <= 'F') and
                        ('0' <= c10 <= '9' or 'A' <= c10 <= 'F') and
                        ('0' <= c11 <= '9' or 'A' <= c11 <= 'F') and
                        ('0' <= c12 <= '9' or 'A' <= c12 <= 'F') and
                        ('0' <= c13 <= '9' or 'A' <= c13 <= 'F') and
                        ('0' <= c14 <= '9' or 'A' <= c14 <= 'F')
                    ):
                        v = o(c2) + o(c7)
        h ^= v + 1
        h = (h * prime) & mask
    return h
