def run(rows: list[str]) -> int:
    # FNV-1a constants
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    # Pre-calculate a translation table for [0-9A-F] validation
    # This is faster than 'all(c in HEX_SET for c in s)'
    valid_hex = "0123456789ABCDEF"
    table = str.maketrans('', '', valid_hex)

    for row in rows:
        v = 0
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be exactly 15
        if len(row) == 15:
            # Check fixed positions and character classes
            # row[0:2] -> [A-Z]{2}
            # row[2:6] -> [0-9]{4}
            # row[6]   -> -
            # row[7:15]-> [0-9A-F]{8}
            if row[6] == '-' and \
               row[0:2].isalpha() and row[0:2].isupper() and \
               row[2:6].isdigit():
                
                hex_part = row[7:15]
                # translate() returns an empty string if all chars are in valid_hex
                if not hex_part.translate(table):
                    v = ord(row[2]) + ord(row[7])

        # FNV-1a update
        h = ((h ^ (v + 1)) * prime) & mask

    return h