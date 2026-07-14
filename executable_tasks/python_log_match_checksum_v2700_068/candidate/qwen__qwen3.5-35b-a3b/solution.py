import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mult = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    match = PAT.match
    for row in rows:
        v = (ord(row[2]) + ord(row[7])) if len(row) == 15 and match(row) else 0
        h = ((h ^ (v + 1)) * mult) & mask
    return h