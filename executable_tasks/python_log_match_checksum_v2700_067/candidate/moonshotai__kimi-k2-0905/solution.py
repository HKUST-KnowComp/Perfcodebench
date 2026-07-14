import string

# Pre-computed tables used by the fast matcher
digits = set(string.digits)
uppercase = set(string.ascii_uppercase)
hexdigits = set(string.digits + 'ABCDEF')


def _is_valid(s: str) -> bool:
    """Return True if s matches the pattern ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$"""
    if len(s) != 15:              # 2+4+1+8 == 15 characters
        return False

    # Positions 0-1 must be uppercase letters
    if (s[0] not in uppercase) or (s[1] not in uppercase):
        return False

    # Positions 2-5 must be decimal digits
    if (s[2] not in digits) or (s[3] not in digits) or \
       (s[4] not in digits) or (s[5] not in digits):
        return False

    # Position 6 must be a dash
    if s[6] != '-':
        return False

    # Positions 7-14 must be hex digits (uppercase)
    if (s[7]  not in hexdigits) or (s[8]  not in hexdigits) or \
       (s[9]  not in hexdigits) or (s[10] not in hexdigits) or \
       (s[11] not in hexdigits) or (s[12] not in hexdigits) or \
       (s[13] not in hexdigits) or (s[14] not in hexdigits):
        return False

    return True


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        v = (ord(row[2]) + ord(row[7])) if _is_valid(row) else 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h