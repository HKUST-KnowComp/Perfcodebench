import urllib.parse


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask64 = (1 << 64) - 1
    unquote = urllib.parse.unquote_plus

    for row in rows:
        u = None
        score = None
        flag = None
        start = 0
        n = len(row)
        while start < n:
            amp = row.find('&', start)
            if amp == -1:
                end = n
            else:
                end = amp
            if end > start:
                eq = row.find('=', start, end)
                if eq != -1:
                    key = row[start:eq]
                    val = row[eq+1:end]
                    if '%' in val or '+' in val:
                        val = unquote(val)
                    if key == 'u' and u is None:
                        if val:
                            u = val
                    elif key == 'score' and score is None:
                        if val:
                            score = val
                    elif key == 'flag' and flag is None:
                        if val:
                            flag = val
            start = end + 1
        u_int = int(u)
        s_int = int(score)
        f_int = int(flag)
        v = (u_int & 2047) + s_int + f_int * 19
        h ^= v + 1
        h *= fnv_prime
        h &= mask64
    return h
