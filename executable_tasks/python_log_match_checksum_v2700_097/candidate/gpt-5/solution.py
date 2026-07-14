def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    ord_ = ord

    for s in rows:
        v = 0
        if len(s) == 15:
            # Two uppercase letters
            if 'A' <= s[0] <= 'Z' and 'A' <= s[1] <= 'Z':
                # Four digits
                if '0' <= s[2] <= '9' and '0' <= s[3] <= '9' and '0' <= s[4] <= '9' and '0' <= s[5] <= '9':
                    # Hyphen
                    if s[6] == '-':
                        # Eight uppercase hex chars
                        c7 = s[7]
                        if ('0' <= c7 <= '9') or ('A' <= c7 <= 'F'):
                            ok = True
                            i = 8
                            while i < 15:
                                c = s[i]
                                if not (('0' <= c <= '9') or ('A' <= c <= 'F')):
                                    ok = False
                                    break
                                i += 1
                            if ok:
                                v = ord_(s[2]) + ord_(c7)
        h ^= v + 1
        h = (h * prime) & mask
    return h
