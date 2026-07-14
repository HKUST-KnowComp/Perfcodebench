def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Manual parsing is significantly faster than parse_qs for simple query strings
        # Expected format: u=...&score=...&flag=...
        u_val = 0
        s_val = 0
        f_val = 0
        
        for part in row.split('&'):
            key, val = part.split('=', 1)
            if key == 'u':
                u_val = int(val)
            elif key == 'score':
                s_val = int(val)
            elif key == 'flag':
                f_val = int(val)
        
        v = (u_val & 2047) + s_val + f_val * 19
        h ^= (v + 1)
        h = (h * FNV_PRIME) & MASK
        
    return h