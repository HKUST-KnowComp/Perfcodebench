import re

PATTERN_BYTES = b'^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$'
def is_valid(row: str) -> bool:
    return re.match(PATTERN_BYTES, row.encode(), re.A) is not None

def fnv_1a_64(row: str) -> int:
    h = 1469598103934665603
    if is_valid(row):
        v = ord(row[2]) + ord(row[7])
        h ^= v + 1
    h *= 1099511628211
    h &= (1 << 64) - 1
    return h

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        h = fnv_1a_64(row)
        h ^= h
    return h