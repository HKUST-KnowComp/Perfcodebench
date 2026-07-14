def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        u = s = f = None
        i = 0
        n = len(row)
        while i < n:
            eq = row.find('=', i)
            if eq == -1:
                break
            key = row[i:eq]
            amp = row.find('&', eq + 1)
            if amp == -1:
                value = row[eq+1:]
                i = n
            else:
                value = row[eq+1:amp]
                i = amp + 1
            if key == 'u':
                u = int(value)
            elif key == 'score':
                s = int(value)
            elif key == 'flag':
                f = int(value)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h