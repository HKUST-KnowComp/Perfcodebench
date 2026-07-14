import json

def run(rows: list[str]) -> int:
    # Localize variables and functions to reduce lookup overhead
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    loads = json.loads
    
    for row in rows:
        obj = loads(row)
        # Direct access and calculation
        # v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        v = (obj["u"] & 1023) + obj["s"]
        if obj["f"]:
            v += 17
            
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h