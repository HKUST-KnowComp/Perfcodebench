def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants for faster access
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Extract 'u' value
        u_idx = row.find('"u":') + 4
        u_end = row.find(',', u_idx)
        u = int(row[u_idx:u_end])

        # Extract 's' value
        s_idx = row.find('"s":') + 4
        s_end = row.find(',', s_idx)
        if s_end == -1:
            s_end = row.find('}', s_idx)
        s = int(row[s_idx:s_end])

        # Extract 'f' value
        f_idx = row.find('"f":') + 4
        f_val = row[f_idx:f_idx + 4]
        f = 17 if 'true' in f_val else 0

        v = (u & 1023) + s + f
        h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
        
    return h