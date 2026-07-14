from urllib.parse import unquote_plus


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    unquote = unquote_plus
    int_ = int
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
            segment = row[start:end]
            if segment:
                eq = segment.find('=')
                if eq != -1:
                    key = segment[:eq]
                    val = segment[eq+1:]
                    if key == 'u' and u is None:
                        u = int_(unquote(val))
                    elif key == 'score' and score is None:
                        score = int_(unquote(val))
                    elif key == 'flag' and flag is None:
                        flag = int_(unquote(val))
            if u is not None and score is not None and flag is not None:
                break
            start = end + 1
        v = (u & 2047) + score + flag * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
