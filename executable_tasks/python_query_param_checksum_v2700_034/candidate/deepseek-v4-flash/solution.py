def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        u = None
        score = None
        flag = None
        # Manual parsing: split by '&', then partition by '='
        for item in row.split('&'):
            key, sep, val = item.partition('=')
            if sep != '=' or not val:
                continue   # malformed: ignore as parse_qs does
            if key == 'u':
                if u is None:
                    u = int(val)
                    if score is not None and flag is not None:
                        break
            elif key == 'score':
                if score is None:
                    score = int(val)
                    if u is not None and flag is not None:
                        break
            elif key == 'flag':
                if flag is None:
                    flag = int(val)
                    if u is not None and score is not None:
                        break
        # All three fields are assumed present; compute hash
        v = (u & 2047) + score + flag * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h