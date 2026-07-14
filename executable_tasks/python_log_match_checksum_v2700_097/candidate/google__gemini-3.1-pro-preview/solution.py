def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    
    # Pre-calculate character sets for faster lookup if needed,
    # but direct range comparisons are usually faster in Python for small sets.
    
    for row in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length: 2 + 4 + 1 + 8 = 15
        if len(row) == 15:
            # Check hyphen first as it's a quick constant check
            if row[6] == '-':
                # Check digits [0-9] at indices 2, 3, 4, 5
                r26 = row[2:6]
                if r26.isdigit():
                    # Check uppercase [A-Z] at indices 0, 1
                    r02 = row[0:2]
                    if r02.isupper() and r02.isalpha():
                        # Check hex [0-9A-F] at indices 7-14
                        r715 = row[7:15]
                        # isxdigit() allows a-f, so we must ensure no lowercase
                        if r715.isxdigit():
                            is_hex_upper = True
                            for i in range(7, 15):
                                c = row[i]
                                # If it's a letter, it must be uppercase A-F
                                if 'a' <= c <= 'f':
                                    is_hex_upper = False
                                    break
                            if is_hex_upper:
                                v = ord(row[2]) + ord(row[7])
        
        # FNV-1a hash update
        h ^= (v + 1)
        h = (h * prime) & mask
        
    return h