import json


# Pre-bind globals/constants for faster local access in the hot loop.
_loads = json.loads
_MASK64 = 0xFFFFFFFFFFFFFFFF
_OFFSET = 1469598103934665603
_PRIME = 1099511628211


def run(rows: list[str]) -> int:
    loads = _loads
    mask = _MASK64
    prime = _PRIME
    h = _OFFSET

    for row in rows:
        obj = loads(row)
        h = ((h ^ (((obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)) + 1)) * prime) & mask

    return h
