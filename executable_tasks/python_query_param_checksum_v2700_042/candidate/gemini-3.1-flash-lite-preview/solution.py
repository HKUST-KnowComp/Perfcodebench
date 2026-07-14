def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Manual parsing is significantly faster than parse_qs for structured query strings
        # Expected format: u=...&score=...&flag=...
        u_start = row.find('u=') + 2
        u_end = row.find('&', u_start)
        u = int(row[u_start:u_end])

        s_start = row.find('score=', u_end) + 6
        s_end = row.find('&', s_start)
        s = int(row[s_start:s_end])

        f_start = row.find('flag=', s_end) + 5
        f = int(row[f_start:])

        v = (u & 2047) + s + f * 19
        h ^= (v + 1)
        h = (h * FNV_PRIME) & MASK
    return h