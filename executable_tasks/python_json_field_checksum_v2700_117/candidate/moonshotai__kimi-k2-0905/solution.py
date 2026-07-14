import ujson


def run(rows: list[str]) -> int:
    # Parse all rows at once to avoid per-row Python JSON overhead
    objs = ujson.loads('[' + ','.join(rows) + ']')
    h = 1469598103934665603
    for obj in objs:
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h