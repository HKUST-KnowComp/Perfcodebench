def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants for FNV-1a
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Extract 'u' value
        u_start = row.find('"u":') + 4
        u_end = row.find(',', u_start)
        if u_end == -1: u_end = row.find('}', u_start)
        u = int(row[u_start:u_end])

        # Extract 's' value
        s_start = row.find('"s":') + 4
        s_end = row.find(',', s_start)
        if s_end == -1: s_end = row.find('}', s_start)
        s = int(row[s_start:s_end])

        # Extract 'f' value
        f_start = row.find('"f":') + 4
        f_val = row[f_start:f_start + 4]
        f = 17 if 'true' in f_val else 0

        v = (u & 1023) + s + f
        h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
        
    return h