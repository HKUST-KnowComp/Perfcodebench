import sys

def run(rows: list[str]) -> int:
    h = 14695981039346656037 & ((1 << 64) - 1)
    for row in rows:
        if (
            len(row) == 14
            and 65 <= ord(row[0])  <= 90   # A-Z
            and 65 <= ord(row[1])  <= 90   # A-Z
            and 48 <= ord(row[2])  <= 57   # 0-9
            and 48 <= ord(row[3])  <= 57   # 0-9
            and 48 <= ord(row[4])  <= 57   # 0-9
            and 48 <= ord(row[5])  <= 57   # 0-9
            and ord(row[6]) == 45          # '-'
            and 48 <= ord(row[7])  <= 57   # 0-9
            and 65 <= ord(row[8])  <= 70   # A-F
            and 48 <= ord(row[9])  <= 57 or 65 <= ord(row[9])  <= 70
            and 48 <= ord(row[10]) <= 57 or 65 <= ord(row[10]) <= 70
            and 48 <= ord(row[11]) <= 57 or 65 <= ord(row[11]) <= 70
            and 48 <= ord(row[12]) <= 57 or 65 <= ord(row[12]) <= 70
            and 48 <= ord(row[13]) <= 57 or 65 <= ord(row[13]) <= 70
        ):
            v = (ord(row[2]) + ord(row[7])) & 0xFF
        else:
            v = 0
        h ^= (v + 1)
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h