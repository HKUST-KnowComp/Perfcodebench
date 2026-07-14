import json

# Bind globals to locals once for faster access in the hot loop.
_loads = json.loads
_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(rows: list[str]) -> int:
    loads = _loads
    h = _FNV_OFFSET
    mask64 = _MASK64
    prime = _FNV_PRIME

    for row in rows:
        obj = loads(row)
        # bool is a subclass of int, so (obj["f"] * 17) is equivalent to
        # (17 if obj["f"] else 0) and avoids a branch.
        v = (obj["u"] & 1023) + obj["s"] + (obj["f"] * 17)
        h = ((h ^ (v + 1)) * prime) & mask64

    return h
