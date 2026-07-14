def run(rows: list[str]) -> int:
    mask = (1 << 64) - 1
    prime = 1099511628211
    h = 1469598103934665603

    for row in rows:
        u = None
        s = None
        f = None
        rf = row.find
        i = 0
        n = len(row)
        while i <= n:
            amp = rf('&', i)
            if amp == -1:
                amp = n
            eq = rf('=', i, amp)
            if eq != -1:
                k = row[i:eq]
                if k == 'u':
                    if u is None:
                        u = int(row[eq + 1:amp])
                elif k == 'score':
                    if s is None:
                        s = int(row[eq + 1:amp])
                elif k == 'flag':
                    if f is None:
                        f = int(row[eq + 1:amp])
            if (u is not None) and (s is not None) and (f is not None):
                break
            i = amp + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
