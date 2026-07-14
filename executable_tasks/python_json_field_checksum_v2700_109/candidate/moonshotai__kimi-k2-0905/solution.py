import sys

# 64-bit mask
MASK64 = (1 << 64) - 1

# Pre-computed constants
INIT = 1469598103934665603
MUL = 1099511628211


def _parse_int(s: bytes, start: int, end: int) -> int:
    """Parse ASCII decimal integer from s[start:end]."""
    v = 0
    sign = 1
    i = start
    if s[i] == 45:  # '-'
        sign = -1
        i += 1
    while i < end:
        v = v * 10 + (s[i] - 48)
        i += 1
    return sign * v


def _parse_bool(s: bytes, start: int, end: int) -> bool:
    """Parse JSON boolean from s[start:end]."""
    return s[start] == 116  # 't'


def run(rows: list[str]) -> int:
    h = INIT
    for row in rows:
        b = row.encode('utf-8')
        n = len(b)

        # Locate "u":
        pos = 5  # "\"u\":" is 5 bytes after opening '{'
        while b[pos] != 58:  # ':'
            pos += 1
        pos += 1
        start_u = pos
        while b[pos] != 44 and b[pos] != 125:  # ',' or '}'
            pos += 1
        u = _parse_int(b, start_u, pos) & 1023

        # Locate "s":
        pos += 1
        while b[pos] != 115:  # 's'
            pos += 1
        pos += 4  # skip "s\":"
        start_s = pos
        while b[pos] != 44 and b[pos] != 125:
            pos += 1
        s = _parse_int(b, start_s, pos)

        # Locate "f":
        pos += 1
        while b[pos] != 102:  # 'f'
            pos += 1
        pos += 4  # skip "f\":"
        start_f = pos
        while b[pos] != 44 and b[pos] != 125 and b[pos] != 32:
            pos += 1
        f = _parse_bool(b, start_f, pos)

        v = u + s + (17 if f else 0)
        h ^= (v + 1)
        h *= MUL
        h &= MASK64
    return h