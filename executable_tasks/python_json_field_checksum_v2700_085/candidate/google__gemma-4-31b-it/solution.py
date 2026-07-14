import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-cache constants and functions to reduce lookup overhead
    mask64 = (1 << 64) - 1
    multiplier = 1099511628211
    loads = json.loads
    
    for row in rows:
        obj = loads(row)
        # Direct access to fields
        # v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        u = obj["u"]
        s = obj["s"]
        f = obj["f"]
        
        v = (u & 1023) + s + (17 if f else 0)
        
        h ^= v + 1
        h = (h * multiplier) & mask64
        
    return h