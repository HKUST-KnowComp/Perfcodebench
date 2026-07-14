import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    if not rows:
        return h
        
    # Joining into a single JSON array and parsing once is significantly faster 
    # than calling json.loads() for each individual row.
    objs = json.loads("[" + ",".join(rows) + "]")
    
    for obj in objs:
        # Combine v + 1 into the conditional to save an addition operation
        h ^= (obj["u"] & 1023) + obj["s"] + (18 if obj["f"] else 1)
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        
    return h
