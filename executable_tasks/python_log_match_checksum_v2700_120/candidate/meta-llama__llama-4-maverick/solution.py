import re

PATTERN = re.compile(rb'^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$', re.ASCII)

def run(rows: list[bytes]) -> int:
    h = 1469598103934665603
    for row in rows:
        if PATTERN.match(row):
            v = row[2] + row[7]
            h ^= v + 1
            h *= 1099511628211
            h &= (1 << 64) - 1
        else:
            h ^= 1
            h *= 1099511628211
            h &= (1 << 64) - 1
    return h