import ujson


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    for obj in ujson.loads('[' + ','.join(rows) + ']'):
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h = (h * mult) & mask
    return h