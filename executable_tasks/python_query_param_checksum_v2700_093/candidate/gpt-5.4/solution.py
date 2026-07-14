def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211

    for row in rows:
        u = 0
        s = 0
        f = 0

        i = 0
        n = len(row)
        while i < n:
            amp = row.find('&', i)
            if amp == -1:
                amp = n

            eq = row.find('=', i, amp)
            if eq != -1:
                key = row[i:eq]
                val_start = eq + 1
                if val_start < amp:
                    if key == 'u':
                        v = 0
                        for j in range(val_start, amp):
                            v = v * 10 + (ord(row[j]) - 48)
                        u = v
                    elif key == 'score':
                        v = 0
                        for j in range(val_start, amp):
                            v = v * 10 + (ord(row[j]) - 48)
                        s = v
                    elif key == 'flag':
                        v = 0
                        for j in range(val_start, amp):
                            v = v * 10 + (ord(row[j]) - 48)
                        f = v

            i = amp + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mul) & mask

    return h
