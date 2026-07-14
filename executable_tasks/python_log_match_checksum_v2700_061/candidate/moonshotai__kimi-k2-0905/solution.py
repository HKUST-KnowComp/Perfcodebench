import re


PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Fast path: validate without regex
        if len(row) != 15:
            v = 0
        elif row[2] != '0' or row[7] != '-':
            v = 0
        elif not row[0:2].isupper():
            v = 0
        elif not row[3:7].isdigit():
            v = 0
        elif not row[8:15].isalnum() or not all(c.isdigit() or c.isupper() for c in row[8:15]):
            v = 0
        else:
            v = ord(row[2]) + ord(row[7])
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h