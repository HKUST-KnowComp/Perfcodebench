import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        obj = json.loads(row)
        # Ensure the required fields exist in the JSON object
        if "u" not in obj or "s" not in obj or "f" not in obj:
            raise ValueError("Missing required field in JSON row")
        
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h