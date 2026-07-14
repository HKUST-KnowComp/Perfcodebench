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
                k0 = row[i]
                klen = eq - i
                val_start = eq + 1
                if val_start < amp:
                    neg = False
                    j = val_start
                    if row[j] == '-':
                        neg = True
                        j += 1
                    val = 0
                    while j < amp:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    if neg:
                        val = -val

                    if k0 == 'u' and klen == 1:
                        u = val
                    elif k0 == 's' and klen == 5 and row[i + 1:eq] == 'core':
                        s = val
                    elif k0 == 'f' and klen == 4 and row[i + 1:eq] == 'lag':
                        f = val

            i = amp + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
