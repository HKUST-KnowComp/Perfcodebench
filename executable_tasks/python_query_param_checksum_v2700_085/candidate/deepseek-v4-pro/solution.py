def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    for row in rows:
        u = None
        score = None
        flag = None
        for part in row.split('&'):
            if '=' in part:
                eq = part.index('=')
                key = part[:eq]
                val = part[eq+1:]
                if key == 'u' and u is None:
                    u = int(val)
                elif key == 'score' and score is None:
                    score = int(val)
                elif key == 'flag' and flag is None:
                    flag = int(val)
                if u is not None and score is not None and flag is not None:
                    break
        v = (u & 2047) + score + flag * 19
        h ^= v + 1
        h = (h * MULT) & MASK
    return h
