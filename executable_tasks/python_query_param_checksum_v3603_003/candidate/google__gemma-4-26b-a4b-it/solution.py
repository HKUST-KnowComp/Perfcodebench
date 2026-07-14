def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1

    for row in rows:
        u = s = f = 0
        # Manual scan to avoid parse_qs overhead
        # Query params are typically key=value&key2=value2
        # We look for 'u=', 'score=', and 'flag='
        
        # Optimization: find substrings directly
        # This is faster than full parsing for specific known keys
        
        # Find 'u'
        idx_u = row.find('u=')
        if idx_u != -1:
            start = idx_u + 2
            end = row.find('&', start)
            u = int(row[start:end] if end != -1 else row[start:])

        # Find 'score'
        idx_s = row.find('score=')
        if idx_s != -1:
            start = idx_s + 6
            end = row.find('&', start)
            s = int(row[start:end] if end != -1 else row[start:])

        # Find 'flag'
        idx_f = row.find('flag=')
        if idx_f != -1:
            start = idx_f + 5
            end = row.find('&', start)
            f = int(row[start:end] if end != -1 else row[start:])

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h