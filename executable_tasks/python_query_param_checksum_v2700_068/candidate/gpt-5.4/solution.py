def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    prime = 1099511628211

    for row in rows:
        u = 0
        s = 0
        f = 0

        n = len(row)
        i = 0
        while i < n:
            j = row.find('=', i)
            if j == -1:
                break
            k = row.find('&', j + 1)
            if k == -1:
                k = n

            key_len = j - i
            val_start = j + 1

            if key_len == 1 and row[i] == 'u':
                val = 0
                for p in range(val_start, k):
                    val = val * 10 + (ord(row[p]) - 48)
                u = val
            elif key_len == 5 and row[i] == 's' and row[i + 1] == 'c' and row[i + 2] == 'o' and row[i + 3] == 'r' and row[i + 4] == 'e':
                val = 0
                for p in range(val_start, k):
                    val = val * 10 + (ord(row[p]) - 48)
                s = val
            elif key_len == 4 and row[i] == 'f' and row[i + 1] == 'l' and row[i + 2] == 'a' and row[i + 3] == 'g':
                val = 0
                for p in range(val_start, k):
                    val = val * 10 + (ord(row[p]) - 48)
                f = val

            i = k + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
