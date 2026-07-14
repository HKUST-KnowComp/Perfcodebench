def run(rows: list[str]) -> int:
    # FNV-1a 64-bit constants
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    for s in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be exactly 15
        if len(s) == 15:
            c0 = s[0]
            c1 = s[1]
            # First two uppercase A-Z
            if 'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z':
                c2 = s[2]
                c3 = s[3]
                c4 = s[4]
                c5 = s[5]
                # Next four digits and a hyphen
                if (
                    '0' <= c2 <= '9' and
                    '0' <= c3 <= '9' and
                    '0' <= c4 <= '9' and
                    '0' <= c5 <= '9' and
                    s[6] == '-'
                ): 
                    c7 = s[7]
                    c8 = s[8]
                    c9 = s[9]
                    c10 = s[10]
                    c11 = s[11]
                    c12 = s[12]
                    c13 = s[13]
                    c14 = s[14]

                    def _is_hex_uc_or_digit(ch: str) -> bool:
                        return ('0' <= ch <= '9') or ('A' <= ch <= 'F')

                    if (
                        _is_hex_uc_or_digit(c7) and
                        _is_hex_uc_or_digit(c8) and
                        _is_hex_uc_or_digit(c9) and
                        _is_hex_uc_or_digit(c10) and
                        _is_hex_uc_or_digit(c11) and
                        _is_hex_uc_or_digit(c12) and
                        _is_hex_uc_or_digit(c13) and
                        _is_hex_uc_or_digit(c14)
                    ):
                        # Only compute ords after full validation
                        v = ord(c2) + ord(c7)

        h ^= (v + 1)
        h = (h * prime) & mask

    return h
