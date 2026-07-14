def run(rows: list[str]) -> int:
    # FNV-1a 64-bit offset basis and prime
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    def parse_int_segment(s: str, start: int, end: int) -> int:
        i = start
        neg = False
        if i < end:
            c = s[i]
            if c == '-':
                neg = True
                i += 1
            elif c == '+':
                i += 1
        # Empty after sign is invalid
        if i >= end:
            raise ValueError("invalid literal for int() with base 10: ''")
        acc = 0
        # Parse digits
        while i < end:
            ch = s[i]
            d = ord(ch) - 48
            if 0 <= d <= 9:
                acc = acc * 10 + d
            else:
                raise ValueError("invalid literal for int() with base 10")
            i += 1
        return -acc if neg else acc

    for row in rows:
        u_val = None
        s_val = None
        f_val = None

        i = 0
        n = len(row)
        while i < n:
            k = i
            eq = row.find('=', k)
            if eq == -1:
                break
            v_start = eq + 1
            amp = row.find('&', v_start)
            if amp == -1:
                v_end = n
                i = n  # move to end to break next iteration
            else:
                v_end = amp
                i = amp + 1

            # Only consider non-empty values (keep_blank_values=False behavior)
            if v_start < v_end:
                keylen = eq - k
                # key == 'u'
                if (
                    u_val is None
                    and keylen == 1
                    and row[k] == 'u'
                ):
                    u_val = parse_int_segment(row, v_start, v_end)
                # key == 'score'
                elif (
                    s_val is None
                    and keylen == 5
                    and row[k] == 's'
                    and row[k + 1] == 'c'
                    and row[k + 2] == 'o'
                    and row[k + 3] == 'r'
                    and row[k + 4] == 'e'
                ):
                    s_val = parse_int_segment(row, v_start, v_end)
                # key == 'flag'
                elif (
                    f_val is None
                    and keylen == 4
                    and row[k] == 'f'
                    and row[k + 1] == 'l'
                    and row[k + 2] == 'a'
                    and row[k + 3] == 'g'
                ):
                    f_val = parse_int_segment(row, v_start, v_end)

            # Early exit if all found
            if u_val is not None and s_val is not None and f_val is not None:
                break

        if u_val is None or s_val is None or f_val is None:
            # Match baseline behavior where missing keys would raise on dict access
            missing = []
            if u_val is None:
                missing.append('u')
            if s_val is None:
                missing.append('score')
            if f_val is None:
                missing.append('flag')
            raise KeyError(', '.join(missing))

        v = (u_val & 2047) + s_val + f_val * 19
        h ^= v + 1
        h = (h * FNV_PRIME) & MASK64

    return h