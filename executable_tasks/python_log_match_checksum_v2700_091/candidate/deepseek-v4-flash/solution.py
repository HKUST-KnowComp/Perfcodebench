def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        v = 0
        if len(row) == 15:
            r0, r1, r2, r3, r4, r5, r6, r7 = row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]
            r8, r9, r10, r11, r12, r13, r14 = row[8], row[9], row[10], row[11], row[12], row[13], row[14]
            if ('A' <= r0 <= 'Z' and 'A' <= r1 <= 'Z' and
                '0' <= r2 <= '9' and '0' <= r3 <= '9' and
                '0' <= r4 <= '9' and '0' <= r5 <= '9' and
                r6 == '-' and
                ('0' <= r7 <= '9' or 'A' <= r7 <= 'F') and
                ('0' <= r8 <= '9' or 'A' <= r8 <= 'F') and
                ('0' <= r9 <= '9' or 'A' <= r9 <= 'F') and
                ('0' <= r10 <= '9' or 'A' <= r10 <= 'F') and
                ('0' <= r11 <= '9' or 'A' <= r11 <= 'F') and
                ('0' <= r12 <= '9' or 'A' <= r12 <= 'F') and
                ('0' <= r13 <= '9' or 'A' <= r13 <= 'F') and
                ('0' <= r14 <= '9' or 'A' <= r14 <= 'F')):
                v = ord(r2) + ord(r7)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h