def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211

    for row in rows:
        n = len(row)
        i = 0
        u = s = f = None

        while i < n:
            key_start = i
            while i < n and row[i] not in '=&':
                i += 1
            key_end = i

            if i >= n or row[i] != '=':
                while i < n and row[i] != '&':
                    i += 1
                if i < n:
                    i += 1
                continue

            key = row[key_start:key_end]
            i += 1
            val_start = i
            while i < n and row[i] != '&':
                i += 1
            val_end = i

            if val_start != val_end:
                if key == 'u' or key == 'score' or key == 'flag':
                    j = val_start
                    neg = False
                    if row[j] == '-':
                        neg = True
                        j += 1
                    val = 0
                    while j < val_end:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    if neg:
                        val = -val
                    if key == 'u' and u is None:
                        u = val
                    elif key == 'score' and s is None:
                        s = val
                    elif key == 'flag' and f is None:
                        f = val

            if i < n:
                i += 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mul) & mask

    return h
