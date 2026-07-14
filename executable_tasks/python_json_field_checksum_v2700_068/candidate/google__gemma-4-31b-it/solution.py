import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    multiplier = 1099511628211
    
    # Pre-caching the loads function for slightly faster access if we had to use it,
    # but manual parsing is faster for this specific schema.
    for row in rows:
        # Using json.loads is the bottleneck. 
        # For the given fields "u", "s", "f", we can optimize.
        obj = json.loads(row)
        # v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        
        # Direct access is faster than repeated lookups
        u = obj['u']
        s = obj['s']
        f = obj['f']
        
        v = (u & 1023) + s + (17 if f else 0)
        
        h ^= v + 1
        h = (h * multiplier) & mask64
        
    return h