def run(rows: list[str]) -> int:
    # FNV-1a 64-bit constants
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    def parse_int_sub(s: str, a: int, b: int) -> int:
        # Parse integer from s[a:b] efficiently; fall back to int() on unexpected chars
        if a >= b:
            # Should not be called with empty substring
            return int(s[a:b])
        i = a
        sign = 1
        ch = s[i]
        if ch == '-':
            sign = -1
            i += 1
        elif ch == '+':
            i += 1
        val = 0
        # Fast path: all digits
        while i < b:
            c = ord(s[i]) - 48
            if c < 0 or c > 9:
                # Fallback to Python's robust int parsing (matches baseline behavior)
                return int(s[a:b])
            val = val * 10 + c
            i += 1
        return val * sign

    for row in rows:
        u_val = None
        s_val = None
        f_val = None

        n = len(row)
        i = 0
        rfind = row.find

        while i <= n:
            amp = rfind('&', i)
            if amp == -1:
                j = n
            else:
                j = amp
            if i >= j:
                # Empty segment, skip
                pass
            else:
                eq = rfind('=', i, j)
                if eq != -1:
                    key_len = eq - i
                    val_start = eq + 1
                    if val_start < j:  # non-empty value (keep_blank_values=False)
                        # Identify key without slicing
                        if u_val is None and key_len == 1 and row[i] == 'u':
                            u_val = parse_int_sub(row, val_start, j)
                        elif s_val is None and key_len == 5 and row[i] == 's' and row[i+1] == 'c' and row[i+2] == 'o' and row[i+3] == 'r' and row[i+4] == 'e':
                            s_val = parse_int_sub(row, val_start, j)
                        elif f_val is None and key_len == 4 and row[i] == 'f' and row[i+1] == 'l' and row[i+2] == 'a' and row[i+3] == 'g':
                            f_val = parse_int_sub(row, val_start, j)
                        # Early exit when all found
                        if u_val is not None and s_val is not None and f_val is not None:
                            break
            if amp == -1:
                break
            i = amp + 1

        if u_val is None or s_val is None or f_val is None:
            # Match baseline behavior where KeyError would be raised when accessing missing key
            raise KeyError("Missing required query parameter")

        v = (u_val & 2047) + s_val + f_val * 19
        h ^= v + 1
        h *= FNV_PRIME
        h &= MASK64

    return h
