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
                val = row[eq + 1:amp]
                if val:
                    if key == 'u':
                        u = int(val)
                    elif key == 'score':
                        s = int(val)
                    elif key == 'flag':
                        f = int(val)

            i = amp + 1

        v = (u & 2047) + s + f * 19
        h = ((h ^ (v + 1)) * mul) & mask

    return h
