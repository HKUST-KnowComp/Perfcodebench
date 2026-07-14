import sys
import json

try:
    import orjson
    def parse_json(s):
        return orjson.loads(s)
except ImportError:
    try:
        import ujson
        def parse_json(s):
            return ujson.loads(s)
    except ImportError:
        def parse_json(s):
            return json.loads(s)


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mult = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    for row in rows:
        obj = parse_json(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h = (h * mult) & mask
    return h
