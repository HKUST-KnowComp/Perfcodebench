def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1

    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual scan to avoid parse_qs overhead
        # We look for key=value pairs separated by & or starting after ?
        start = 0
        if '?' in row:
            start = row.find('?') + 1
        
        # Split by '&' manually to avoid full regex or complex parsing
        # We iterate through segments to find our specific keys
        for segment in row[start:].split('&'):
            if not segment:
                continue
            
            if '=' in segment:
                k, v_str = segment.split('=', 1)
                if k == 'u':
                    u = int(v_str)
                elif k == 'score':
                    s = int(v_str)
                elif k == 'flag':
                    f = int(v_str)

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h