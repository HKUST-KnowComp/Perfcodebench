def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        u = 0
        s = 0
        f = 0
        i = 0
        n = len(row)
        while i < n:
            # Find key start
            j = i
            while j < n and row[j] != '=':
                j += 1
            key = row[i:j]
            j += 1  # skip '='
            # Find value end
            k = j
            while k < n and row[k] != '&':
                k += 1
            val = row[j:k]
            if key == 'u':
                u = int(val)
            elif key == 'score':
                s = int(val)
            elif key == 'flag':
                f = int(val)
            i = k + 1
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
