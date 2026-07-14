import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    # Localize functions to reduce lookup overhead
    loads = json.loads
    
    for row in rows:
        obj = loads(row)
        # Direct access and minimal operations
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = (h ^ (v + 1)) & mask
        h = (h * multiplier) & mask
        
    return h