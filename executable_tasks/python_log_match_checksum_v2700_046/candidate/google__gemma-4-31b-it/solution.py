def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    # Pre-calculate constants for the loop
    # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
    # Length must be 2 + 4 + 1 + 8 = 15
    for row in rows:
        if len(row) == 15 and \
           'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z' and \
           '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and \
           '0' <= row[4] <= '9' and '0' <= row[5] <= '9' and \
           row[6] == '-' and \
           all( ('0' <= c <= '9' or 'A' <= c <= 'F') for c in row[7:]):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h

# Further optimization: The 'all()' is slow. Unrolling the hex check.

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        if len(row) == 15 and \
           'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z' and \
           '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and \
           '0' <= row[4] <= '9' and '0' <= row[5] <= '9' and \
           row[6] == '-' and \
           ('0' <= row[7] <= '9' or 'A' <= row[7] <= 'F') and \
           ('0' <= row[8] <= '9' or 'A' <= row[8] <= 'F') and \
           ('0' <= row[9] <= '9' or 'A' <= row[9] <= 'F') and \
           ('0' <= row[10] <= '9' or 'A' <= row[10] <= 'F') and \
           ('0' <= row[11] <= '9' or 'A' <= row[11] <= 'F') and \
           ('0' <= row[12] <= '9' or 'A' <= row[12] <= 'F') and \
           ('0' <= row[13] <= '9' or 'A' <= row[13] <= 'F') and \
           ('0' <= row[14] <= '9' or 'A' <= row[14] <= 'F'):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h