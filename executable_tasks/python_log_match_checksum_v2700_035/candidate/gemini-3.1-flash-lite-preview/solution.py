def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        if len(row) == 15 and \
           'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z' and \
           row[2:6].isdigit() and row[6] == '-' and \
           all('0' <= c <= '9' or 'A' <= c <= 'F' for c in row[7:]):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h