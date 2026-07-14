def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    
    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual scan to avoid parse_qs overhead
        # Query strings are typically k1=v1&k2=v2
        # We look for 'u=', 'score=', and 'flag='
        
        # Find start of params (after '?')
        start = row.find('?')
        if start == -1:
            start = 0
        else:
            start += 1
            
        # Split by '&' to get pairs
        pairs = row[start:].split('&')
        for pair in pairs:
            if not pair:
                continue
            
            # Find '=' position
            eq_idx = pair.find('=')
            if eq_idx == -1:
                continue
                
            key = pair[:eq_idx]
            val = pair[eq_idx+1:]
            
            if not val:
                continue

            if key == 'u':
                u = int(val)
            elif key == 'score':
                s = int(val)
            elif key == 'flag':
                f = int(val)

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h