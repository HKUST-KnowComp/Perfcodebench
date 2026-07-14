def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for row in rows:
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be 2 + 4 + 1 + 8 = 15
        is_match = False
        if len(row) == 15:
            # Check [A-Z]{2}
            if (65 <= ord(row[0]) <= 90) and (65 <= ord(row[1]) <= 90):
                # Check [0-9]{4}
                if (48 <= ord(row[2]) <= 57) and (48 <= ord(row[3]) <= 57) and \
                   (48 <= ord(row[4]) <= 57) and (48 <= ord(row[5]) <= 57):
                    # Check -
                    if row[6] == '-':
                        # Check [0-9A-F]{8}
                        # We only need to check if they are in 0-9 or A-F
                        # 0-9: 48-57, A-F: 65-70
                        match_hex = True
                        for i in range(7, 15):
                            c = ord(row[i])
                            if not ((48 <= c <= 57) or (65 <= c <= 70)):
                                match_hex = False
                                break
                        if match_hex:
                            is_match = True

        v = (ord(row[2]) + ord(row[7])) if is_match else 0
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h