import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    FNV = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    match = PAT.match
    for row in rows:
        v = (ord(row[2]) + ord(row[7])) if match(row) else 0
        h ^= v + 1
        h *= FNV
        h &= MASK
    return h