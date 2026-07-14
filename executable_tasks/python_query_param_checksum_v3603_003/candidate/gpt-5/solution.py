def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        u = None
        s = None
        f = None

        i = 0
        n = len(row)
        find = row.find

        while True:
            if u is not None and s is not None and f is not None:
                break

            j = find('&', i)
            if j == -1:
                j = n

            eq = row.find('=', i, j)
            if eq != -1:
                # Match only the needed keys without allocating the key substring
                # Key length is eq - i
                kl = eq - i
                if u is None and kl == 1 and row[i] == 'u':
                    val = row[eq + 1 : j]
                    u = int(val)
                elif s is None and kl == 5 and row.startswith('score', i, eq):
                    val = row[eq + 1 : j]
                    s = int(val)
                elif f is None and kl == 4 and row.startswith('flag', i, eq):
                    val = row[eq + 1 : j]
                    f = int(val)

            if j == n:
                break
            i = j + 1

        # Compute the value and update the hash
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
