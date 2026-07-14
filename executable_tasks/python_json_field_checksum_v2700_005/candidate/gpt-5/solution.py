def run(rows: list[str]) -> int:
    # 64-bit FNV-1a constants
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    fnv_prime = 1099511628211

    for s in rows:
        n = len(s)
        # Helper: skip whitespace
        pos = 0
        # Parse field 'u'
        i = s.find('"u"', 0)
        j = s.find(':', i + 3)
        pos = j + 1
        while pos < n and s[pos] <= ' ':
            pos += 1
        neg = False
        if s[pos] == '-':
            neg = True
            pos += 1
        val = 0
        # parse integer digits
        while pos < n:
            ch = s[pos]
            d = ord(ch) - 48
            if 0 <= d <= 9:
                val = val * 10 + d
                pos += 1
            else:
                break
        u = -val if neg else val

        # Parse field 's'
        i = s.find('"s"', pos)
        if i == -1:
            i = s.find('"s"', 0)
        j = s.find(':', i + 3)
        pos = j + 1
        while pos < n and s[pos] <= ' ':
            pos += 1
        neg = False
        if s[pos] == '-':
            neg = True
            pos += 1
        val = 0
        while pos < n:
            ch = s[pos]
            d = ord(ch) - 48
            if 0 <= d <= 9:
                val = val * 10 + d
                pos += 1
            else:
                break
        s_val = -val if neg else val

        # Parse field 'f'
        i = s.find('"f"', pos)
        if i == -1:
            i = s.find('"f"', 0)
        j = s.find(':', i + 3)
        pos = j + 1
        while pos < n and s[pos] <= ' ':
            pos += 1
        f_val = s[pos] == 't'  # true starts with 't'

        v = (u & 1023) + s_val + (17 if f_val else 0)
        h ^= v + 1
        h = (h * fnv_prime) & mask

    return h
