MASK64 = (1 << 64) - 1
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def run(rows: list[str]) -> int:
    h = FNV_OFFSET
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
                        for ch in row[val_start:amp]:
                            v = v * 10 + (ord(ch) - 48)
                        u = v
                    elif key == 'score':
                        v = 0
                        j = val_start
                        while j < amp:
                            v = v * 10 + (ord(row[j]) - 48)
                            j += 1
                        s = v
                    elif key == 'flag':
                        v = 0
                        j = val_start
                        while j < amp:
                            v = v * 10 + (ord(row[j]) - 48)
                            j += 1
                        f = v
            i = amp + 1

        h ^= ((u & 2047) + s + f * 19) + 1
        h = (h * FNV_PRIME) & MASK64
    return h
