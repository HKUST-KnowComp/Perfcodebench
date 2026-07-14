def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Fast path: check length first
        if len(row) == 15:
            # Check fixed positions: AA0000-00000000
            # Positions: 0,1 = A-Z; 2-5 = 0-9; 6 = '-'; 7-14 = 0-9,A-F
            c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14 = row
            
            if (c6 == '-' and
                'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z' and
                '0' <= c2 <= '9' and '0' <= c3 <= '9' and
                '0' <= c4 <= '9' and '0' <= c5 <= '9' and
                ('0' <= c7 <= '9' or 'A' <= c7 <= 'F') and
                ('0' <= c8 <= '9' or 'A' <= c8 <= 'F') and
                ('0' <= c9 <= '9' or 'A' <= c9 <= 'F') and
                ('0' <= c10 <= '9' or 'A' <= c10 <= 'F') and
                ('0' <= c11 <= '9' or 'A' <= c11 <= 'F') and
                ('0' <= c12 <= '9' or 'A' <= c12 <= 'F') and
                ('0' <= c13 <= '9' or 'A' <= c13 <= 'F') and
                ('0' <= c14 <= '9' or 'A' <= c14 <= 'F')):
                v = ord(c2) + ord(c7)
            else:
                v = 0
        else:
            v = 0
        
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h