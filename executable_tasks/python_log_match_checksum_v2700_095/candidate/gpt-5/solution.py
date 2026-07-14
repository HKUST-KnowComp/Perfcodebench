def run(rows: list[str]) -> int:
    # FNV-1a parameters (64-bit)
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        v = 0
        if len(row) == 15:
            s = row
            c0 = s[0]
            c1 = s[1]
            # First two uppercase letters
            if 'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z':
                c2 = s[2]
                c3 = s[3]
                c4 = s[4]
                c5 = s[5]
                # Four digits
                if '0' <= c2 <= '9' and '0' <= c3 <= '9' and '0' <= c4 <= '9' and '0' <= c5 <= '9':
                    # Hyphen
                    if s[6] == '-':
                        c7 = s[7]
                        c8 = s[8]
                        c9 = s[9]
                        c10 = s[10]
                        c11 = s[11]
                        c12 = s[12]
                        c13 = s[13]
                        c14 = s[14]
                        # Helper inline predicate for hex uppercase digit
                        def _is_hex(ch: str) -> bool:
                            return ('0' <= ch <= '9') or ('A' <= ch <= 'F')
                        if (_is_hex(c7) and _is_hex(c8) and _is_hex(c9) and _is_hex(c10)
                                and _is_hex(c11) and _is_hex(c12) and _is_hex(c13) and _is_hex(c14)):
                            v = ord(c2) + ord(c7)
        h ^= (v + 1)
        h *= fnv_prime
        h &= mask
    return h
