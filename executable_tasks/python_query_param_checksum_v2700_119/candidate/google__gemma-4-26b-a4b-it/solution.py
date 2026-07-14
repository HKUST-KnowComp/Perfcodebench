def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 2047
    mod = (1 << 64) - 1
    
    for row in rows:
        # Manual scanning is significantly faster than parse_qs for specific keys
        # We look for 'u=', 'score=', and 'flag=' patterns
        
        # Find 'u'
        u_idx = row.find('u=')
        if u_idx == -1: continue
        u_start = u_idx + 2
        u_end = u_start
        while u_end < len(row) and row[u_end].isdigit():
            u_end += 1
        u = int(row[u_start:u_end])

        # Find 'score'
        s_idx = row.find('score=')
        if s_idx == -1: continue
        s_start = s_idx + 6
        s_end = s_start
        while s_end < len(row) and row[s_end].isdigit():
            s_end += 1
        s = int(row[s_start:s_end])

        # Find 'flag'
        f_idx = row.find('flag=')
        if f_idx == -1: continue
        f_start = f_idx + 5
        f_end = f_start
        while f_end < len(row) and row[f_end].isdigit():
            f_end += 1
        f = int(row[f_start:f_end])

        v = (u & mask) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mod
        
    return h