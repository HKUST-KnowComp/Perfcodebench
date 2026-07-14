def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for row in rows:
        u = s = f = 0
        # Manual split is faster than parse_qs for specific keys
        for pair in row.split('&'):
            if not pair:
                continue
            k, v = pair.split('=', 1)
            if k == 'u':
                u = int(v)
            elif k == 'score':
                s = int(v)
            elif k == 'flag':
                f = int(v)
        
        val = (u & 2047) + s + f * 19
        h ^= val + 1
        h = (h * multiplier) & mask
        
    return h