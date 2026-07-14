def run(rows: list[str]) -> int:
    h = 1469598103934665603
    # Pre-calculate ordinals for faster comparison
    A, Z = ord('A'), ord('Z')
    zero, nine = ord('0'), ord('9')
    a, f = ord('a'), ord('f')
    A_hex, F_hex = ord('A'), ord('F')

    for row in rows:
        # Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        # Length must be 15
        if len(row) == 15 and \
           A <= ord(row[0]) <= Z and A <= ord(row[1]) <= Z and \
           zero <= ord(row[2]) <= nine and zero <= ord(row[3]) <= nine and \
           zero <= ord(row[4]) <= nine and zero <= ord(row[5]) <= nine and \
           row[6] == '-' and \
           all((zero <= ord(c) <= nine or A_hex <= ord(c) <= F_hex) for c in row[7:]):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        
        h ^= v + 1
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h