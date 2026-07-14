import json

def _extract(pairs):
    u = s = f = None
    for key, value in pairs:
        if key == 'u':
            u = value
        elif key == 's':
            s = value
        elif key == 'f':
            f = value
    return (u, s, f)

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    decoder = json.JSONDecoder(object_pairs_hook=_extract)
    for row in rows:
        u, s, f = decoder.decode(row)
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h