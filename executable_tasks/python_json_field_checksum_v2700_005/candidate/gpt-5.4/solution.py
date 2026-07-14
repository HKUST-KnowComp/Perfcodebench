import json


# Bind globals to locals once for faster access in the hot loop.
_loads = json.loads
_MASK64 = (1 << 64) - 1
_OFFSET = 1469598103934665603
_PRIME = 1099511628211


def run(rows: list[str]) -> int:
    loads = _loads
    h = _OFFSET
    prime = _PRIME
    mask64 = _MASK64

    for row in rows:
        obj = loads(row)
        # bool is a subclass of int, so this exactly matches:
        # (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        v = (obj["u"] & 1023) + obj["s"] + (obj["f"] * 17)
        h = ((h ^ (v + 1)) * prime) & mask64

    return h
