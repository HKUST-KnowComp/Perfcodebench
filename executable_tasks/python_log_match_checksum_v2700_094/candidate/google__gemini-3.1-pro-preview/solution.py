import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    _ord = ord
    _match = PAT.match
    for row in rows:
        if _match(row):
            h ^= _ord(row[2]) + _ord(row[7]) + 1
        else:
            h ^= 1
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h
