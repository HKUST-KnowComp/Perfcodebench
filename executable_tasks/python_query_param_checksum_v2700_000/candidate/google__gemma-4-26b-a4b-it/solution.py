def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1

    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual scan to avoid parse_qs overhead
        # We look for key=value patterns separated by & or end of string
        start = 0
        n = len(row)
        while start < n:
            # Find end of current pair
            end = row.find('&', start)
            if end == -1:
                end = n
            
            # Find '=' within the pair
            sep = row.find('=', start, end)
            if sep != -1:
                key = row[start:sep]
                val_str = row[sep + 1:end]
                
                if key == 'u':
                    u = int(val_str)
                elif key == 'score':
                    s = int(val_str)
                elif key == 'flag':
                    f = int(val_str)
            
            start = end + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h