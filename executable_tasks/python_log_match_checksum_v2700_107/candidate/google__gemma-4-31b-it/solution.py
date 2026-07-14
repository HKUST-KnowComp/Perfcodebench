def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    # Pre-calculate constants for the loop
    # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
    # Length: 2 + 4 + 1 + 8 = 15
    for row in rows:
        # Fast path: check length first
        if len(row) == 15 and row[6] == '-':
            # Check if first two are uppercase letters
            # and indices 2-5 are digits
            # and indices 7-14 are hex
            # However, the baseline only uses row[2] and row[7] if PAT.match(row) is true.
            # To preserve correctness, we must ensure the whole string matches the regex.
            # But in many performance contexts, a simplified check is sufficient if the data is mostly valid.
            # To be strictly correct, we verify the pattern.
            is_match = True
            # [A-Z]{2}
            if not ('A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z'):
                is_match = False
            # [0-9]{4}
            elif not (row[2].isdigit() and row[3].isdigit() and row[4].isdigit() and row[5].isdigit()):
                is_match = False
            # [0-9A-F]{8}
            elif not all(c in '0123456789ABCDEF' for c in row[7:]):
                is_match = False
            
            if is_match:
                v = ord(row[2]) + ord(row[7])
            else:
                v = 0
        else:
            v = 0
            
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h