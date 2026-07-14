def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Manual parsing is significantly faster than parse_qs for fixed structures
        # Expected format: u=...&score=...&flag=...
        # We split by '&' and then '=' to extract values
        u, s, f = 0, 0, 0
        for part in row.split('&'):
            key, val = part.split('=', 1)
            if key == 'u':
                u = int(val)
            elif key == 'score':
                s = int(val)
            elif key == 'flag':
                f = int(val)
        
        v = (u & 2047) + s + f * 19
        h ^= (v + 1)
        h = (h * FNV_PRIME) & MASK
    return h