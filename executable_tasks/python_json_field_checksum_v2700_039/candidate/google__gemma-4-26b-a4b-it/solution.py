import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    
    for row in rows:
        # Manual scanning is faster than json.loads for specific keys in flat structures
        # We look for the keys directly in the string to avoid full object instantiation
        
        # Extract 'u'
        u_idx = row.find('"u":')
        u_start = u_idx + 4
        u_end = u_start
        while u_end < len(row) and (row[u_end].isdigit() or row[u_end] == '-' or row[u_end] == '.'):
            u_end += 1
        u_val = int(row[u_start:u_end])

        # Extract 's'
        s_idx = row.find('"s":')
        s_start = s_idx + 4
        s_end = s_start
        while s_end < len(row) and (row[s_end].isdigit() or row[s_end] == '-' or row[s_end] == '.'):
            s_end += 1
        s_val = int(row[s_start:s_end])

        # Extract 'f'
        f_idx = row.find('"f":')
        # 'f' is boolean, so we check for 'true'
        f_val = 1 if row[f_idx + 4 : f_idx + 8] == 'true' else 0

        v = (u_val & 1023) + s_val + (17 if f_val else 0)
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h