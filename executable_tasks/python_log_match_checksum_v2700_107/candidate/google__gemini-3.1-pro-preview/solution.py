import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$", re.ASCII)


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    match = PAT.match
    for row in rows:
        if match(row):
            h ^= ord(row[2]) + ord(row[7]) + 1
        else:
            h ^= 1
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h
