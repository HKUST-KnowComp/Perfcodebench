import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    
    for row in rows:
        # Manual scanning is faster than json.loads for specific field extraction
        # We look for the keys "u", "s", and "f"
        
        # Extract 'u'
        u_idx = row.find('"u":')
        u_start = u_idx + 4
        u_end = row.find(',', u_start)
        if u_end == -1:
            u_end = row.find('}', u_start)
        u_val = int(row[u_start:u_end].strip())

        # Extract 's'
        s_idx = row.find('"s":')
        s_start = s_idx + 4
        s_end = row.find(',', s_start)
        if s_end == -1:
            s_end = row.find('}', s_start)
        s_val = int(row[s_start:s_end].strip())

        # Extract 'f'
        f_idx = row.find('"f":')
        f_start = f_idx + 4
        f_end = row.find(',', f_start)
        if f_end == -1:
            f_end = row.find('}', f_start)
        # Check if value is true (boolean)
        f_val = row[f_start:f_end].strip() == 'true'

        v = (u_val & 1023) + s_val + (17 if f_val else 0)
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h