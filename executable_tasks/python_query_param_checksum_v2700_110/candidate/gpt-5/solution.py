from urllib.parse import unquote_plus as _unquote_plus


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        u = None
        s = None
        f = None

        i = 0
        n = len(row)
        while True:
            amp = row.find('&', i)
            part_end = n if amp == -1 else amp

            if part_end > i:
                eq = row.find('=', i, part_end)
                if eq == -1:
                    key = row[i:part_end]
                    val = ''
                else:
                    key = row[i:eq]
                    val = row[eq + 1:part_end]

                if u is None and key == 'u':
                    try:
                        u = int(val)
                    except ValueError:
                        u = int(_unquote_plus(val))
                elif s is None and key == 'score':
                    try:
                        s = int(val)
                    except ValueError:
                        s = int(_unquote_plus(val))
                elif f is None and key == 'flag':
                    try:
                        f = int(val)
                    except ValueError:
                        f = int(_unquote_plus(val))

                if u is not None and s is not None and f is not None:
                    # Found all needed params; stop scanning this row
                    break

            if amp == -1:
                break
            i = amp + 1

        # Compute hash using the same formula as baseline
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= fnv_prime
        h &= mask

    return h
