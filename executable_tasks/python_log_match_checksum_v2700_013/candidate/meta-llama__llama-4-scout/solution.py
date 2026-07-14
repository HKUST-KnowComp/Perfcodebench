import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if PAT.match(row):
            v = (ord(row[2]) + ord(row[7]))
            h ^= v + 1
            h *= 1099511628211
            h &= (1 << 64) - 1
    return h