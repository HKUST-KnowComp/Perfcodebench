def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    
    # Pre-calculate hex set for faster lookup
    hex_chars = set("0123456789ABCDEF")
    
    for row in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be exactly 15
        if len(row) == 15:
            # Check fixed separator first as it's a cheap fail
            if row[6] == '-':
                # Check [A-Z]{2}
                r0, r1 = row[0], row[1]
                if 'A' <= r0 <= 'Z' and 'A' <= r1 <= 'Z':
                    # Check [0-9]{4}
                    r2345 = row[2:6]
                    if r2345.isdigit():
                        # Check [0-9A-F]{8}
                        r7_15 = row[7:]
                        is_hex = True
                        for char in r7_15:
                            if char not in hex_chars:
                                is_hex = False
                                break
                        if is_hex:
                            # Pattern matched
                            v = ord(row[2]) + ord(row[7])
        
        h ^= (v + 1)
        h = (h * mult) & mask
        
    return h