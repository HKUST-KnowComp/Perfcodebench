import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate constants for FNV-1a
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for row in rows:
        # Manual extraction to avoid full json.loads overhead
        # Expected format: {"u": int, "s": int, "f": bool}
        # Find keys and extract values using string slicing
        u_idx = row.find('"u":') + 4
        u_end = row.find(',', u_idx)
        u = int(row[u_idx:u_end])

        s_idx = row.find('"s":', u_end) + 4
        s_end = row.find(',', s_idx)
        s = int(row[s_idx:s_end])

        f_idx = row.find('"f":', s_end) + 4
        f_end = row.find('}', f_idx)
        f = row[f_idx:f_end].strip() == 'true'

        v = (u & 1023) + s + (17 if f else 0)
        h ^= (v + 1)
        h = (h * FNV_PRIME) & MASK
        
    return h