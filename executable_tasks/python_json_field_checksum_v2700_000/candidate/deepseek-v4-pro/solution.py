import sys

try:
    import orjson
    def _loads(x):
        return orjson.loads(x)
except ImportError:
    import json
    def _loads(x):
        return json.loads(x)


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MUL = 1099511628211
    for row in rows:
        obj = _loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h = (h * MUL) & MASK
    return h
