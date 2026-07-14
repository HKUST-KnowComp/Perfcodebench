def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        u = s = f = 0
        # Manually parse the query string to avoid the overhead of parse_qs
        for pair in row.split('&'):
            if not pair:
                continue
            # Split only once per pair
            kv = pair.split('=', 1)
            if len(kv) < 2:
                continue
            
            key = kv[0]
            val = kv[1]
            
            if key == 'u':
                u = int(val)
            elif key == 'score':
                s = int(val)
            elif key == 'flag':
                f = int(val)
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h