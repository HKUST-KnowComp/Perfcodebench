def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    
    # Pre-define hex set for faster lookup than range comparisons for multiple chars
    HEX_CHARS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
                 'A', 'B', 'C', 'D', 'E', 'F'}

    for row in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length: 2 + 4 + 1 + 8 = 15
        if len(row) == 15:
            if (row[6] == '-' and
                '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and
                '0' <= row[4] <= '9' and '0' <= row[5] <= '9' and
                'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z' and
                row[7] in HEX_CHARS and row[8] in HEX_CHARS and
                row[9] in HEX_CHARS and row[10] in HEX_CHARS and
                row[11] in HEX_CHARS and row[12] in HEX_CHARS and
                row[13] in HEX_CHARS and row[14] in HEX_CHARS):
                v = ord(row[2]) + ord(row[7])
        
        h = ((h ^ (v + 1)) * prime) & mask
        
    return h