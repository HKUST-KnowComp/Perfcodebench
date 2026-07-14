def run(rows: list[str]) -> int:
    # Fast scanner for query parameters extracting only 'u', 'score', and 'flag'.
    # Computes a 64-bit FNV-1a checksum as in the baseline.
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    def parse_int(s: str, start: int, end: int) -> int:
        # Parse an integer from s[start:end] with optional leading '+' or '-'.
        i = start
        neg = False
        if i < end:
            ch = s[i]
            if ch == '-':
                neg = True
                i += 1
            elif ch == '+':
                i += 1
        val = 0
        # Assume well-formed numeric characters for performance.
        while i < end:
            val = val * 10 + (ord(s[i]) - 48)
            i += 1
        return -val if neg else val

    h = FNV_OFFSET
    for row in rows:
        u = None
        s_val = None
        f = None

        i = 0
        n = len(row)
        # Iterate over key=value pairs separated by '&'
        while True:
            amp = row.find('&', i)
            end = n if amp == -1 else amp
            eq = row.find('=', i, end)
            if eq != -1:
                key_len = eq - i
                # Match 'u'
                if key_len == 1 and row[i] == 'u':
                    if u is None:
                        u = parse_int(row, eq + 1, end)
                # Match 'score'
                elif key_len == 5 and row[i] == 's' and row[i + 1] == 'c' and row[i + 2] == 'o' and row[i + 3] == 'r' and row[i + 4] == 'e':
                    if s_val is None:
                        s_val = parse_int(row, eq + 1, end)
                # Match 'flag'
                elif key_len == 4 and row[i] == 'f' and row[i + 1] == 'l' and row[i + 2] == 'a' and row[i + 3] == 'g':
                    if f is None:
                        f = parse_int(row, eq + 1, end)
            # Early exit if all found
            if u is not None and s_val is not None and f is not None:
                break
            if amp == -1:
                break
            i = amp + 1

        # Compute checksum update
        v = (u & 2047) + s_val + f * 19
        h ^= v + 1
        h = (h * FNV_PRIME) & MASK64

    return h
