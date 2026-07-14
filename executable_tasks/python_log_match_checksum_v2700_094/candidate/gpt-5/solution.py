def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    ord_ = ord
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
                if '0' <= c2 <= '9' and '0' <= c3 <= '9' and '0' <= c4 <= '9' and '0' <= c5 <= '9':
                    if s[6] == '-':
                        t7 = s[7]
                        t8 = s[8]
                        t9 = s[9]
                        t10 = s[10]
                        t11 = s[11]
                        t12 = s[12]
                        t13 = s[13]
                        t14 = s[14]
                        if (('0' <= t7 <= '9' or 'A' <= t7 <= 'F') and
                            ('0' <= t8 <= '9' or 'A' <= t8 <= 'F') and
                            ('0' <= t9 <= '9' or 'A' <= t9 <= 'F') and
                            ('0' <= t10 <= '9' or 'A' <= t10 <= 'F') and
                            ('0' <= t11 <= '9' or 'A' <= t11 <= 'F') and
                            ('0' <= t12 <= '9' or 'A' <= t12 <= 'F') and
                            ('0' <= t13 <= '9' or 'A' <= t13 <= 'F') and
                            ('0' <= t14 <= '9' or 'A' <= t14 <= 'F')):
                            v = ord_(c2) + ord_(t7)
        h ^= v + 1
        h = (h * prime) & mask
    return h
