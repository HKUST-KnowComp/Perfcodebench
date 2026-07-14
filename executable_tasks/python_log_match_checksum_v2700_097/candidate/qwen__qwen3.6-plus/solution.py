import re

PAT = re.compile(r"[A-Z]{2}[0-9]{4}-[0-9A-F]{8}")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MUL = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    for row in rows:
        if PAT.fullmatch(row):
            h ^= ord(row[2]) + ord(row[7]) + 1
        else:
            h ^= 1
        h = (h * MUL) & MASK
    return h