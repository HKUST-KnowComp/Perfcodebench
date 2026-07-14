def run(rows: list[str]) -> int:
    # Fast specialized JSON field scanner for keys: "u" (int), "s" (int), "f" (bool)
    # Computes checksum using the same FNV-1a mixing as baseline.
    M = (1 << 64) - 1
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211

    WS = ' \t\r\n'
    KEY_U = '"u"'
    KEY_S = '"s"'
    KEY_F = '"f"'

    h = FNV_OFFSET

    for row in rows:
        s = row
        n = len(s)

        # Parse integer value after key starting at index j
        def parse_int(j: int) -> tuple[int, int]:
            sign = 1
            if j < n and s[j] == '-':
                sign = -1
                j += 1
            val = 0
            # parse digits
            while j < n:
                ch = s[j]
                d = ord(ch) - 48
                if 0 <= d <= 9:
                    val = val * 10 + d
                    j += 1
                else:
                    break
            return sign * val, j

        # Find start index of value for a given key pattern
        def find_value_start(key: str) -> int:
            i = s.find(key, 0)
            klen = 3  # len('"x"')
            while i != -1:
                j = i + klen
                # skip whitespace to ':'
                while j < n and s[j] in WS:
                    j += 1
                if j < n and s[j] == ':':
                    j += 1
                    while j < n and s[j] in WS:
                        j += 1
                    return j
                # Not a key position; continue searching
                i = s.find(key, j)
            return -1

        # Find and parse u
        j = find_value_start(KEY_U)
        # assuming valid input contains the key
        u_val, _ = parse_int(j)

        # Find and parse s
        j = find_value_start(KEY_S)
        s_val, _ = parse_int(j)

        # Find and parse f (boolean)
        j = find_value_start(KEY_F)
        # Expect 'true' or 'false'
        # JSON booleans are lowercase
        f_char = s[j] if j < n else 'f'
        if f_char == 't':
            f_val = True
        else:
            # default false if not 't'
            f_val = False

        v = (u_val & 1023) + s_val + (17 if f_val else 0)
        h ^= (v + 1)
        h *= FNV_PRIME
        h &= M

    return h
