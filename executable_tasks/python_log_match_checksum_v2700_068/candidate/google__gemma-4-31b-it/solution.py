def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be 2 + 4 + 1 + 8 = 15
        if len(row) == 15 and row[6] == '-':
            # Check [A-Z]{2}
            if 'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z':
                # Check [0-9]{4}
                if '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9':
                    # Check [0-9A-F]{8}
                    valid_hex = True
                    for i in range(7, 15):
                        char = row[i]
                        if not ('0' <= char <= '9' or 'A' <= char <= 'F'):
                            valid_hex = False
                            break
                    if valid_hex:
                        v = ord(row[2]) + ord(row[7])
                    else:
                        v = 0
                else:
                    v = 0
            else:
                v = 0
        else:
            v = 0
            
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h