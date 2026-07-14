import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Extract values using string slicing/finding to avoid full json.loads overhead
        # Keys are 'u', 's', 'f'. Assuming standard JSON formatting as per task context.
        u_idx = row.find('"u":') + 4
        u_end = row.find(',', u_idx)
        if u_end == -1: u_end = row.find('}', u_idx)
        u = int(row[u_idx:u_end])

        s_idx = row.find('"s":') + 4
        s_end = row.find(',', s_idx)
        if s_end == -1: s_end = row.find('}', s_idx)
        s = int(row[s_idx:s_end])

        f_idx = row.find('"f":') + 4
        f_val = row[f_idx:f_idx+4].strip()
        f = f_val.startswith('true')

        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h