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

            key = row[i:j]
            val_start = j + 1

            if key == 'u':
                x = 0
                for c in row[val_start:k]:
                    x = x * 10 + (ord(c) - 48)
                u = x
            elif key == 'score':
                x = 0
                for c in row[val_start:k]:
                    x = x * 10 + (ord(c) - 48)
                s = x
            elif key == 'flag':
                x = 0
                for c in row[val_start:k]:
                    x = x * 10 + (ord(c) - 48)
                f = x

            i = k + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
