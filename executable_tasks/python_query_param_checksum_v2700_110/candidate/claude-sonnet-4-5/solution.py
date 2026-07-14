def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        # Fast manual parsing - find key positions
        u_pos = row.find('u=')
        score_pos = row.find('score=')
        flag_pos = row.find('flag=')
        
        # Extract u value
        u_start = u_pos + 2
        u_end = row.find('&', u_start)
        if u_end == -1:
            u_end = len(row)
        u = int(row[u_start:u_end])
        
        # Extract score value
        score_start = score_pos + 6
        score_end = row.find('&', score_start)
        if score_end == -1:
            score_end = len(row)
        s = int(row[score_start:score_end])
        
        # Extract flag value
        flag_start = flag_pos + 5
        flag_end = row.find('&', flag_start)
        if flag_end == -1:
            flag_end = len(row)
        f = int(row[flag_start:flag_end])
        
        # Compute hash
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    
    return h