import re

PAT = re.compile(r"[A-Z]{2}[0-9]{4}-[0-9A-F]{8}")


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    match = PAT.fullmatch
    for row in rows:
        if len(row) == 15 and match(row):
            h = ((h ^ (ord(row[2]) + ord(row[7]) + 1)) * prime) & mask
        else:
            h = ((h ^ 1) * prime) & mask
    return h
