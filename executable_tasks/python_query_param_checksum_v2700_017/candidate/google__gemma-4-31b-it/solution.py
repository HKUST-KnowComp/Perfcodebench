def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual parsing of query string to avoid parse_qs overhead
        for pair in row.split('&'):
            if not pair:
                continue
            k, _, v = pair.partition('=')
            if not v:
                continue
            
            if k == 'u':
                u = int(v)
            elif k == 'score':
                s = int(v)
            elif k == 'flag':
                f = int(v)
        
        val = (u & 2047) + s + f * 19
        h ^= val + 1
        h = (h * 1099511628211) & mask
        
    return h