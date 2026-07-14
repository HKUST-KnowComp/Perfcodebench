import re

_HEX = frozenset(b'0123456789ABCDEF')
_UPPER = frozenset(range(ord('A'), ord('Z') + 1))
_DIGIT = frozenset(range(ord('0'), ord('9') + 1))
_MASK = (1 << 64) - 1

def _valid(row: str) -> bool:
    if len(row) != 15:
        return False
    if row[6] != '-':
        return False
    o = ord
    if o(row[0]) not in _UPPER or o(row[1]) not in _UPPER:
        return False
    if o(row[2]) not in _DIGIT or o(row[3]) not in _DIGIT or o(row[4]) not in _DIGIT or o(row[5]) not in _DIGIT:
        return False
    rb = row.encode('latin-1')
    for i in range(7, 15):
        if rb[i] not in _HEX:
            return False
    return True

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    valid = _valid
    for row in rows:
        if valid(row):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= _MASK
    return h
