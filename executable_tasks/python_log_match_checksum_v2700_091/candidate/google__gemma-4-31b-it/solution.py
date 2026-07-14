def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for row in rows:
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be 2 + 4 + 1 + 8 = 15
        if len(row) == 15 and \
           row[0].isupper() and row[1].isupper() and \
           row[2].isdigit() and row[3].isdigit() and row[4].isdigit() and row[5].isdigit() and \
           row[6] == '-' and \
           all(c.isdigit() or ('A' <= c <= 'F') for c in row[7:]):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
            
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h