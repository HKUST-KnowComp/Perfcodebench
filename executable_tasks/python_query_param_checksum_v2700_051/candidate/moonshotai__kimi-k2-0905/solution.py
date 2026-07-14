from typing import List

def run(rows: List[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        # parse "u=<int>&score=<int>&flag=<int>"
        # u
        i += 2  # skip "u="
        v = 0
        while i < n and row[i] != '&':
            v = v * 10 + (ord(row[i]) - 48)
            i += 1
        u = v
        # score
        i += 7  # skip "&score="
        v = 0
        while i < n and row[i] != '&':
            v = v * 10 + (ord(row[i]) - 48)
            i += 1
        s = v
        # flag
        i += 6  # skip "&flag="
        v = 0
        while i < n:
            v = v * 10 + (ord(row[i]) - 48)
            i += 1
        f = v
        # checksum step
        vv = (u & 2047) + s + f * 19
        h ^= (vv + 1)
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h