import json

# Bind globals to locals once for faster access in the hot loop.
_loads = json.loads
_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(rows: list[str]) -> int:
    h = _FNV_OFFSET
    loads = _loads
    mask64 = _MASK64
    prime = _FNV_PRIME

    for row in rows:
        obj = loads(row)
        h ^= ((obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)) + 1
        h = (h * prime) & mask64

    return h
