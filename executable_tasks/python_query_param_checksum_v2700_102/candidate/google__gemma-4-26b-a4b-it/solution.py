def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        # Manual scanning is significantly faster than parse_qs for specific keys
        # We look for 'u=', 'score=', and 'flag=' patterns
        
        # Find 'u'
        u_idx = row.find('u=')
        if u_idx == -1: continue
        u_start = u_idx + 2
        u_end = u_start
        while u_end < len(row) and row[u_end] != '&':
            u_end += 1
        u_val = int(row[u_start:u_end])

        # Find 'score'
        s_idx = row.find('score=')
        if s_idx == -1: continue
        s_start = s_idx + 6
        s_end = s_start
        while s_end < len(row) and row[s_end] != '&':
            s_end += 1
        s_val = int(row[s_start:s_end])

        # Find 'flag'
        f_idx = row.find('flag=')
        if f_idx == -1: continue
        f_start = f_idx + 5
        f_end = f_start
        while f_end < len(row) and row[f_end] != '&':
            f_end += 1
        f_val = int(row[f_start:f_end])

        v = (u_val & 2047) + s_val + f_val * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h