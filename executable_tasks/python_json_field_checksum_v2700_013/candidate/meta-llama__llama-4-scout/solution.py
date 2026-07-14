import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        obj = json.loads(row)
        # Directly access and process relevant fields to minimize overhead
        u = obj['u'] & 1023
        s = obj['s']
        f = obj['f']
        v = u + s + (17 if f else 0)
        # Use a consistent and efficient hashing approach
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h