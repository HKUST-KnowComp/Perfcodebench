import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for row in rows:
        # Manual extraction to avoid json.loads overhead
        # Finding "u":
        u_start = row.find('"u":') + 4
        u_end = row.find(',', u_start)
        if u_end == -1: u_end = row.find('}', u_start)
        u_val = int(row[u_start:u_end])
        
        # Finding "s":
        s_start = row.find('"s":') + 4
        s_end = row.find(',', s_start)
        if s_end == -1: s_end = row.find('}', s_start)
        s_val = int(row[s_start:s_end])
        
        # Finding "f":
        f_start = row.find('"f":') + 4
        # f is boolean, check if it starts with 't'
        f_val = 17 if row[f_start] == 't' else 0
        
        v = (u_val & 1023) + s_val + f_val
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h