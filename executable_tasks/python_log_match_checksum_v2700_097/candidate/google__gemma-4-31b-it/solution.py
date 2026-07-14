def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    # Pre-calculate constants for the loop
    # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
    # Length: 2 + 4 + 1 + 8 = 15
    for row in rows:
        # Fast length check first
        if len(row) == 15:
            # Check specific indices to validate pattern quickly
            # row[0:2] are A-Z, row[2:6] are 0-9, row[6] is '-', row[7:15] are 0-9A-F
            # The original code only needs ord(row[2]) and ord(row[7]) if it matches
            # We can use isdigit() and isupper() for fast validation
            if (row[6] == '-' and 
                row[2:6].isdigit() and 
                row[0:2].isupper() and 
                row[7:15].isalnum()):
                # Further refine the 0-9A-F check if necessary, 
                # but isalnum() is a good fast filter for the hex part
                # To be strictly correct with [0-9A-F], we check the hex range
                # However, the baseline used re.match, so we must be exact.
                # Let's use a more precise check for the hex part.
                is_hex = True
                for char in row[7:15]:
                    if not ('0' <= char <= '9' or 'A' <= char <= 'F'):
                        is_hex = False
                        break
                if is_hex:
                    v = ord(row[2]) + ord(row[7])
                else:
                    v = 0
            else:
                v = 0
        else:
            v = 0
        
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h