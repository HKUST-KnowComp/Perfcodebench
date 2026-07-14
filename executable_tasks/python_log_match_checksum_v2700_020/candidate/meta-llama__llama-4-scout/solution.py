import re

PAT = re.compile(r"^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$")

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if len(row) >= 14 and row[0:2].isalpha() and row[2:6].isdigit() and row[6:14].isalnum() and row[6:10].isdigit() and row[10:14] == '-'+row[14:22].upper():
            v = (ord(row[2]) + ord(row[7])) 
            h ^= v + 1
            h *= 1099511628211
            h &= (1 << 64) - 1
    return h