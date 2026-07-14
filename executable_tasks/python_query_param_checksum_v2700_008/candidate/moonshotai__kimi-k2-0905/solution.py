from typing import List

def run(rows: List[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        # parse u
        while i < n and row[i] != '=': i += 1
        i += 1
        while i < n and row[i] != '&':
            u = u * 10 + (ord(row[i]) - 48)
            i += 1
        i += 1  # skip '&'
        # parse score
        while i < n and row[i] != '=': i += 1
        i += 1
        while i < n and row[i] != '&':
            s = s * 10 + (ord(row[i]) - 48)
            i += 1
        i += 1  # skip '&'
        # parse flag
        while i < n and row[i] != '=': i += 1
        i += 1
        while i < n:
            f = f * 10 + (ord(row[i]) - 48)
            i += 1
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h