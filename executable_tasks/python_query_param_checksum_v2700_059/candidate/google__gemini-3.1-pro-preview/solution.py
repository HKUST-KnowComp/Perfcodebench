def run(rows: list[str]) -> int:
    # FNV-1a constants for 64-bit
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    
    # Localize variables for faster access in the loop
    for row in rows:
        u = 0
        score = 0
        flag = 0
        
        # Manual parsing is significantly faster than parse_qs
        # split('&') handles the key-value pairs
        for part in row.split('&'):
            if not part:
                continue
            # Check first character to quickly identify the key
            # u=, score=, flag=
            first_char = part[0]
            if first_char == 'u':
                # part is 'u=VAL'
                u = int(part[2:])
            elif first_char == 's':
                # part is 'score=VAL'
                score = int(part[6:])
            elif first_char == 'f':
                # part is 'flag=VAL'
                flag = int(part[5:])
        
        # Calculate the value based on the extracted parameters
        v = (u & 2047) + score + flag * 19
        
        # Update the hash using the FNV-1a pattern provided in the baseline
        h ^= (v + 1)
        h = (h * prime) & mask
        
    return h