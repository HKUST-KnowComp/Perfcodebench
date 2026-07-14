from urllib.parse import unquote


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = score = flag = None
        start = 0
        while True:
            amp = row.find('&', start)
            if amp == -1:
                seg = row[start:]
            else:
                seg = row[start:amp]
            eq = seg.find('=')
            if eq != -1:
                key = seg[:eq]
                val = seg[eq+1:]
                if key == 'u':
                    u = int(unquote(val))
                elif key == 'score':
                    score = int(unquote(val))
                elif key == 'flag':
                    flag = int(unquote(val))
            if amp == -1:
                break
            start = amp + 1
        if u is None or score is None or flag is None:
            raise ValueError("Missing required field")
        v = (u & 2047) + score + flag * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
