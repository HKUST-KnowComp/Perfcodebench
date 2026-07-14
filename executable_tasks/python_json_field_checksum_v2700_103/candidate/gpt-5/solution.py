def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    for s in rows:
        # Parse field 'u'
        i = s.find('"u"')
        i = s.find(':', i + 3) + 1
        j = i
        # skip whitespace
        while True:
            c = s[j]
            if c == ' ' or c == '\t' or c == '\n' or c == '\r':
                j += 1
            else:
                break
        neg = False
        if s[j] == '-':
            neg = True
            j += 1
        n = 0
        while True:
            d = ord(s[j]) - 48
            if 0 <= d <= 9:
                n = n * 10 + d
                j += 1
            else:
                break
        if neg:
            n = -n
        u_val = n

        # Parse field 's'
        i = s.find('"s"')
        i = s.find(':', i + 3) + 1
        j = i
        while True:
            c = s[j]
            if c == ' ' or c == '\t' or c == '\n' or c == '\r':
                j += 1
            else:
                break
        neg = False
        if s[j] == '-':
            neg = True
            j += 1
        n = 0
        while True:
            d = ord(s[j]) - 48
            if 0 <= d <= 9:
                n = n * 10 + d
                j += 1
            else:
                break
        if neg:
            n = -n
        s_val = n

        # Parse field 'f'
        i = s.find('"f"')
        i = s.find(':', i + 3) + 1
        j = i
        while True:
            c = s[j]
            if c == ' ' or c == '\t' or c == '\n' or c == '\r':
                j += 1
            else:
                break
        # JSON booleans: true/false
        f_add = 17 if s[j] == 't' else 0

        v = (u_val & 1023) + s_val + f_add
        h ^= (v + 1)
        h = (h * prime) & mask

    return h
