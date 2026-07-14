def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    prime = 1099511628211

    for row in rows:
        u = 0
        s = 0
        f = 0

        i = 0
        n = len(row)
        while i < n:
            amp = row.find('&', i)
            if amp == -1:
                end = n
                next_i = n
            else:
                end = amp
                next_i = amp + 1

            eq = row.find('=', i, end)
            if eq != -1:
                key = row[i:eq]
                val_start = eq + 1
                if val_start < end:
                    val = row[val_start:end]
                    if key == 'u':
                        u = int(val)
                    elif key == 'score':
                        s = int(val)
                    elif key == 'flag':
                        f = int(val)

            i = next_i

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
