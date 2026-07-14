import json


def run(rows: list[str]) -> int:
    # Parse all rows at once as a single JSON array to avoid per-row json.loads overhead.
    json_arr = '[' + ','.join(rows) + ']'
    objects = json.loads(json_arr)

    h = 1469598103934665603
    MASK = 0xFFFFFFFFFFFFFFFF
    MULT = 1099511628211
    for obj in objects:
        u = obj["u"]
        s = obj["s"]
        f = obj["f"]
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h = (h * MULT) & MASK
    return h
