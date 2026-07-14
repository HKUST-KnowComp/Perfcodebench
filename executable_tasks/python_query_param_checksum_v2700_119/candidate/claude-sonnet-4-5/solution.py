def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        # Manual parsing - find each parameter directly
        u_idx = row.find('u=')
        if u_idx != -1:
            u_start = u_idx + 2
            u_end = row.find('&', u_start)
            u = int(row[u_start:u_end] if u_end != -1 else row[u_start:])
        else:
            u = 0
        
        s_idx = row.find('score=')
        if s_idx != -1:
            s_start = s_idx + 6
            s_end = row.find('&', s_start)
            s = int(row[s_start:s_end] if s_end != -1 else row[s_start:])
        else:
            s = 0
        
        f_idx = row.find('flag=')
        if f_idx != -1:
            f_start = f_idx + 5
            f_end = row.find('&', f_start)
            f = int(row[f_start:f_end] if f_end != -1 else row[f_start:])
        else:
            f = 0
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    
    return h