import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")
MASK = (1 << 64) - 1
PRIME = 1099511628211

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    pat_match = PAT.match
    for row in rows:
        if len(row) == 15 and pat_match(row):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h = ((h ^ (v + 1)) * PRIME) & MASK
    return h