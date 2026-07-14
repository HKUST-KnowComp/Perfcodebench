import json

try:
    import orjson
    _USE_ORJSON = True
except ImportError:
    _USE_ORJSON = False
    try:
        import ujson
        _USE_UJSON = True
    except ImportError:
        _USE_UJSON = False


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211

    if _USE_ORJSON:
        loads = orjson.loads
        for row in rows:
            obj = loads(row)
            v = (obj[b"u"] & 1023) + obj[b"s"] + (17 if obj[b"f"] else 0)
            h = ((h ^ (v + 1)) * mult) & mask
    elif not _USE_ORJSON and 'ujson' in dir():
        loads = ujson.loads
        for row in rows:
            obj = loads(row)
            v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
            h = ((h ^ (v + 1)) * mult) & mask
    else:
        loads = json.loads
        for row in rows:
            obj = loads(row)
            v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
            h = ((h ^ (v + 1)) * mult) & mask

    return h
