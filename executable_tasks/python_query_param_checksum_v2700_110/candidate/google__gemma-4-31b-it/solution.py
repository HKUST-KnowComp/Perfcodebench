def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        u = 0
        score = 0
        flag = 0
        
        # Manual parsing is significantly faster than parse_qs for specific keys
        for pair in row.split('&'):
            if not pair:
                continue
            k, _, v = pair.partition('=')
            if k == 'u':
                u = int(v)
            elif k == 'score':
                score = int(v)
            elif k == 'flag':
                flag = int(v)
        
        v_val = (u & 2047) + score + flag * 19
        h ^= v_val + 1
        h = (h * 1099511628211) & mask
        
    return h