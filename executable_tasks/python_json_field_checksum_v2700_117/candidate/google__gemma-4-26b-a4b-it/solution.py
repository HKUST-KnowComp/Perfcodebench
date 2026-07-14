import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    
    for row in rows:
        # Manual scanning is faster than json.loads for specific fields
        # We look for the keys "u", "s", and "f"
        # This assumes the JSON structure is relatively consistent as per the task
        
        # Find 'u'
        u_idx = row.find('"u":') + 4
        while row[u_idx] in ' 	

': u_idx += 1
        u_val = 0
        while u_idx < len(row) and row[u_idx].isdigit():
            u_val = u_val * 10 + (ord(row[u_idx]) - 48)
            u_idx += 1
            
        # Find 's'
        s_idx = row.find('"s":') + 4
        while row[s_idx] in ' 	

': s_idx += 1
        s_val = 0
        while s_idx < len(row) and row[s_idx].isdigit():
            s_val = s_val * 10 + (ord(row[s_idx]) - 48)
            s_idx += 1
            
        # Find 'f'
        f_idx = row.find('"f":') + 4
        while row[f_idx] in ' 	

': f_idx += 1
        # f is boolean: true or false
        f_val = 1 if row[f_idx:f_idx+4] == 'true' else 0
        
        v = (u_val & 1023) + s_val + (17 if f_val else 0)
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h