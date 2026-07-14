def run(rows: list[str]) -> int:
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    
    # Pre-define character sets for validation
    HEX_CHARS = "0123456789ABCDEF"
    DIGITS = "0123456789"
    UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

    for row in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length: 2 + 4 + 1 + 8 = 15
        if len(row) == 15 and row[6] == '-':
            # Check [A-Z]{2} at indices 0, 1
            if row[0] in UPPER and row[1] in UPPER:
                # Check [0-9]{4} at indices 2, 3, 4, 5
                # strip() is faster than all() for bulk character validation
                if not row[2:6].strip(DIGITS):
                    # Check [0-9A-F]{8} at indices 7-14
                    if not row[7:15].strip(HEX_CHARS):
                        v = ord(row[2]) + ord(row[7])
        
        # FNV-1a style hash update
        h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
        
    return h