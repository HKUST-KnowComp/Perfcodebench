import json

try:
    import orjson
    _loads = orjson.loads
except ImportError:
    try:
        import ujson
        _loads = ujson.loads
    except ImportError:
        _loads = json.loads


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    loads = _loads
    mask = (1 << 64) - 1
    mult = 1099511628211
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h = (h * mult) & mask
    return h
