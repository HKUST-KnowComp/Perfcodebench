def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask_2047 = 2047
    mask_64 = (1 << 64) - 1
    mult = 1099511628211
    
    for row in rows:
        # Fast manual parsing for specific keys
        u = s = f = 0
        
        # Parse row character by character
        i = 0
        length = len(row)
        
        while i < length:
            # Find key start
            if row[i] == 'u' and i + 2 < length and row[i + 1] == '=':
                # Parse u value
                i += 2
                num = 0
                while i < length and row[i].isdigit():
                    num = num * 10 + (ord(row[i]) - 48)
                    i += 1
                u = num
            elif row[i] == 's' and i + 6 < length and row[i:i+6] == 'score=':
                # Parse score value
                i += 6
                num = 0
                while i < length and row[i].isdigit():
                    num = num * 10 + (ord(row[i]) - 48)
                    i += 1
                s = num
            elif row[i] == 'f' and i + 5 < length and row[i:i+5] == 'flag=':
                # Parse flag value
                i += 5
                num = 0
                while i < length and row[i].isdigit():
                    num = num * 10 + (ord(row[i]) - 48)
                    i += 1
                f = num
            else:
                i += 1
        
        v = (u & mask_2047) + s + f * 19
        h ^= v + 1
        h *= mult
        h &= mask_64
    
    return h