import re

PAT = re.compile(r"[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    match = PAT.match
    for row in rows:
        h ^= ord(row[2]) + ord(row[7]) + 1 if match(row) else 1
        h *= 1099511628211
        h &= 18446744073709551615
    return h
