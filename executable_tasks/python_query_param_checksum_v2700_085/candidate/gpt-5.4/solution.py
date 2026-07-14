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
            if eq != -1 and eq + 1 < amp:
                k0 = row[i]
                if k0 == 'u' and eq - i == 1:
                    val = 0
                    j = eq + 1
                    while j < amp:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    u = val
                elif k0 == 's' and eq - i == 5 and row[i + 1:eq] == 'core':
                    val = 0
                    j = eq + 1
                    while j < amp:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    s = val
                elif k0 == 'f' and eq - i == 4 and row[i + 1:eq] == 'lag':
                    val = 0
                    j = eq + 1
                    while j < amp:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    f = val

            i = amp + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
