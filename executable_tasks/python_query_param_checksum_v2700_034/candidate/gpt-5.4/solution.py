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
            if j < 0:
                break
            k = row.find('&', j + 1)
            if k < 0:
                k = n

            key_len = j - i
            val_start = j + 1

            if key_len == 1:
                if row[i] == 'u':
                    val = 0
                    for p in range(val_start, k):
                        val = val * 10 + (ord(row[p]) - 48)
                    u = val
            elif key_len == 4:
                if row[i:j] == 'flag':
                    val = 0
                    for p in range(val_start, k):
                        val = val * 10 + (ord(row[p]) - 48)
                    f = val
            elif key_len == 5:
                if row[i:j] == 'score':
                    val = 0
                    for p in range(val_start, k):
                        val = val * 10 + (ord(row[p]) - 48)
                    s = val

            i = k + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
