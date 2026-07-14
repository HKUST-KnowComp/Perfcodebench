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
            j = row.find('=', i)
            if j == -1:
                break
            k = row.find('&', j + 1)
            if k == -1:
                k = n

            key_len = j - i
            val_start = j + 1

            if key_len == 1:
                if row[i] == 'u':
                    x = 0
                    for p in range(val_start, k):
                        x = x * 10 + (ord(row[p]) - 48)
                    u = x
                elif row[i] == 'f':
                    x = 0
                    for p in range(val_start, k):
                        x = x * 10 + (ord(row[p]) - 48)
                    f = x
            elif key_len == 5 and row[i:i + 5] == 'score':
                x = 0
                for p in range(val_start, k):
                    x = x * 10 + (ord(row[p]) - 48)
                s = x

            i = k + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
