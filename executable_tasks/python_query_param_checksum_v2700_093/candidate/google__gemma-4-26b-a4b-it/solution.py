def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1

    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Find the start of query params (after '?')
        start = row.find('?')
        if start == -1:
            start = 0
        else:
            start += 1
            
        # Manual scan for key-value pairs to avoid parse_qs overhead
        # We look for 'u=', 'score=', and 'flag=' specifically
        # The query string is typically k1=v1&k2=v2
        
        # Optimization: Instead of full parsing, we use find to jump to keys
        # This is faster because we only care about 3 specific keys
        
        # Find 'u'
        idx = row.find('u=', start)
        while idx != -1:
            # Ensure it's a full key (start of string or preceded by &)
            if idx == start or row[idx-1] == '&':
                end = idx + 2
                while end < len(row) and row[end].isdigit():
                    end += 1
                u = int(row[idx+2:end])
                break
            idx = row.find('u=', idx + 1)
            
        # Find 'score'
        idx = row.find('score=', start)
        while idx != -1:
            if idx == start or row[idx-1] == '&':
                end = idx + 6
                while end < len(row) and row[end].isdigit():
                    end += 1
                s = int(row[idx+6:end])
                break
            idx = row.find('score=', idx + 1)

        # Find 'flag'
        idx = row.find('flag=', start)
        while idx != -1:
            if idx == start or row[idx-1] == '&':
                end = idx + 5
                while end < len(row) and row[end].isdigit():
                    end += 1
                f = int(row[idx+5:end])
                break
            idx = row.find('flag=', idx + 1)

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h