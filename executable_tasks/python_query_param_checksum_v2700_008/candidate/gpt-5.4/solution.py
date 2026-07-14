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
                amp = n

            eq = row.find('=', i, amp)
            if eq != -1:
                key = row[i:eq]
                val_start = eq + 1
                if val_start < amp:
                    val = 0
                    j = val_start
                    while j < amp:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1

                    c0 = key[0]
                    if c0 == 'u':
                        u = val
                    elif c0 == 's':
                        s = val
                    else:
                        f = val

            i = amp + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
