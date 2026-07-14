import re

PATTERN_BYTES = b'^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$'
def is_match(row: str) -> bool:
    return re.match(PATTERN_BYTES, row.encode(), re.A) is not None

def fnv_1a_64(data: int, h: int = 1469598103934665603) -> int:
    h ^= data
    h *= 1099511628211
    return h & ((1 << 64) - 1)

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if is_match(row):
            v = ord(row[2]) + ord(row[7])
            h = fnv_1a_64(v + 1, h)
        else:
            h = fnv_1a_64(1, h)
    return h