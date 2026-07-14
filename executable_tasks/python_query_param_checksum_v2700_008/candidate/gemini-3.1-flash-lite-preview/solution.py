def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Manual parsing is significantly faster than parse_qs for fixed schemas
        # Expected format: u=...&score=...&flag=...
        u_val = 0
        s_val = 0
        f_val = 0
        
        # Extract u
        u_start = row.find('u=') + 2
        u_end = row.find('&', u_start)
        u_val = int(row[u_start:u_end])
        
        # Extract score
        s_start = row.find('score=', u_end) + 6
        s_end = row.find('&', s_start)
        s_val = int(row[s_start:s_end])
        
        # Extract flag
        f_start = row.find('flag=', s_end) + 5
        f_val = int(row[f_start:])
        
        v = (u_val & 2047) + s_val + f_val * 19
        h ^= (v + 1)
        h = (h * FNV_PRIME) & MASK
        
    return h