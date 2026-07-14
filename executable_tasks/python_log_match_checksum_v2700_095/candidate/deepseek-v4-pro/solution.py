import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")
MASK = (1 << 64) - 1
MULT = 1099511628211
SEED = 1469598103934665603

def run(rows: list[str]) -> int:
    h = SEED
    pat = PAT
    mask = MASK
    mult = MULT
    for row in rows:
        if len(row) == 15 and pat.match(row):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h = (h * mult) & mask
    return h
