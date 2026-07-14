import json

def run(rows: list[str]) -> int:
    # FNV-1a 64-bit constants
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    
    # Localize function for speed
    load = json.loads
    
    for row in rows:
        obj = load(row)
        # Extract values directly
        # v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        u = obj["u"]
        s = obj["s"]
        f = obj["f"]
        
        v = (u & 1023) + s + (17 if f else 0)
        
        h ^= (v + 1)
        h = (h * prime) & mask
        
    return h