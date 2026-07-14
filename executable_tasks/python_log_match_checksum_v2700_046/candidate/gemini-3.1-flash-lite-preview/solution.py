def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
    # Length: 2 + 4 + 1 + 8 = 15
    for row in rows:
        valid = False
        if len(row) == 15:
            if (row[0].isupper() and row[1].isupper() and
                row[2:6].isdigit() and row[6] == '-' and
                all(c.isdigit() or 'A' <= c <= 'F' for c in row[7:])):
                valid = True
        
        v = (ord(row[2]) + ord(row[7])) if valid else 0
        h ^= v + 1
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h