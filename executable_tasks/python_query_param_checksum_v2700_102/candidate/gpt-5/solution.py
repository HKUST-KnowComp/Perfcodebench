def run(rows: list[str]) -> int:
    # 64-bit FNV-1a constants
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        n = len(row)
        i = 0
        u = None
        s = None
        f = None
        find = row.find
        startswith = row.startswith

        while True:
            amp = find('&', i)
            seg_end = n if amp == -1 else amp

            eq = find('=', i, seg_end)
            if eq != -1:
                klen = eq - i
                # Only process keys we care about and only first non-blank value
                if klen == 1:
                    if row[i] == 'u' and u is None:
                        if eq + 1 < seg_end:
                            u = int(row[eq + 1:seg_end])
                elif klen == 5 and startswith('score', i, eq):
                    if s is None:
                        if eq + 1 < seg_end:
                            s = int(row[eq + 1:seg_end])
                elif klen == 4 and startswith('flag', i, eq):
                    if f is None:
                        if eq + 1 < seg_end:
                            f = int(row[eq + 1:seg_end])

            if (u is not None and s is not None and f is not None) or amp == -1:
                break
            i = amp + 1

        # Compute checksum update
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * fnv_prime) & mask

    return h
