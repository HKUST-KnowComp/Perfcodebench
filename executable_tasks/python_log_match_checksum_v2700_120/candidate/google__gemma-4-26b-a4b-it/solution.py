def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be 2 + 4 + 1 + 8 = 15
        is_match = True
        if len(row) != 15:
            is_match = False
        else:
            # [A-Z]{2}
            if not (65 <= ord(row[0]) <= 90 and 65 <= ord(row[1]) <= 90):
                is_match = False
            # [0-9]{4}
            elif not (48 <= ord(row[2]) <= 57 and 48 <= ord(row[3]) <= 57 and 
                      48 <= ord(row[4]) <= 57 and 48 <= ord(row[5]) <= 57):
                is_match = False
            # -
            elif row[6] != '-':
                is_match = False
            # [0-9A-F]{8}
            else:
                for i in range(7, 15):
                    c = ord(row[i])
                    if not (48 <= c <= 57 or 65 <= c <= 70):
                        is_match = False
                        break
        
        v = (ord(row[2]) + ord(row[7])) if is_match else 0
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h