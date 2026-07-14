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
            key_start = i
            # Find '=' or '&' or end
            while i < n and row[i] != '=' and row[i] != '&':
                i += 1
            key_end = i
            if i < n and row[i] == '=':
                i += 1  # skip '='
                val_start = i
                while i < n and row[i] != '&':
                    i += 1
                val_end = i
            else:
                val_start = i
                val_end = i
            if i < n:
                i += 1  # skip '&'
            key_len = key_end - key_start
            if key_len == 1 and row[key_start] == 'u':
                u = int(row[val_start:val_end])
            elif key_len == 5 and row[key_start:key_end] == 'score':
                s = int(row[val_start:val_end])
            elif key_len == 4 and row[key_start:key_end] == 'flag':
                f = int(row[val_start:val_end])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
