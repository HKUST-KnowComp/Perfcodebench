def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Manual parsing - find each parameter directly
        u_idx = row.find('u=')
        u_end = row.index('&', u_idx)
        u = int(row[u_idx + 2:u_end])
        
        s_idx = row.find('score=')
        s_end = row.index('&', s_idx)
        s = int(row[s_idx + 6:s_end])
        
        f_idx = row.find('flag=')
        # flag might be last, so handle both cases
        try:
            f_end = row.index('&', f_idx)
            f = int(row[f_idx + 5:f_end])
        except ValueError:
            f = int(row[f_idx + 5:])
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
