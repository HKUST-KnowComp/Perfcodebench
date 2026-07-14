import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    match = PAT.match
    mask = 0xFFFFFFFFFFFFFFFF
    mul = 1099511628211
    for row in rows:
        if match(row):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h = ((h ^ (v + 1)) * mul) & mask
    return h
