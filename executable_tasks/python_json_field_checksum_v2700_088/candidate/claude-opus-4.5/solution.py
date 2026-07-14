import json

try:
    import orjson
    _USE_ORJSON = True
except ImportError:
    _USE_ORJSON = False


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    _mul = 1099511628211
    _mask = (1 << 64) - 1
    
    if _USE_ORJSON:
        loads = orjson.loads
        for row in rows:
            obj = loads(row)
            v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
            h ^= v + 1
            h = (h * _mul) & _mask
    else:
        loads = json.loads
        for row in rows:
            obj = loads(row)
            v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
            h ^= v + 1
            h = (h * _mul) & _mask
    
    return h
