def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Extract 'u' value
        u_start = row.find('"u":') + 4
        u_end = row.find(',', u_start)
        u = int(row[u_start:u_end])
        
        # Extract 's' value
        s_start = row.find('"s":') + 4
        s_end = row.find(',', s_start)
        if s_end == -1:
            s_end = row.find('}', s_start)
        s = int(row[s_start:s_end])
        
        # Extract 'f' value
        f_start = row.find('"f":') + 4
        f_val = row[f_start:f_start + 4]
        f = True if 'true' in f_val else False
        
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h