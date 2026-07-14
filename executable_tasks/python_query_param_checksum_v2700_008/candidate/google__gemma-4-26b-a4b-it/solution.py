def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    
    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual scan to avoid parse_qs overhead
        # Expected format: key=value&key2=value2
        # We look for 'u=', 'score=', and 'flag='
        
        # Split by '&' to get pairs
        pairs = row.split('&')
        for pair in pairs:
            if not pair:
                continue
            
            # Find the '=' position
            sep_idx = pair.find('=')
            if sep_idx == -1:
                continue
            
            key = pair[:sep_idx]
            val_str = pair[sep_idx + 1:]
            
            if not val_str:
                continue
                
            if key == 'u':
                u = int(val_str)
            elif key == 'score':
                s = int(val_str)
            elif key == 'flag':
                f = int(val_str)
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h