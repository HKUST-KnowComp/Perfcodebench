from typing import List

def run(rows: List[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        # scan u
        while i < n and row[i] != "u": i += 1
        i += 2  # skip "u="
        while i < n and row[i].isdigit():
            u = u * 10 + (ord(row[i]) - 48)
            i += 1
        # scan score
        while i < n and row[i] != "s": i += 1
        i += 6  # skip "score="
        while i < n and row[i].isdigit():
            s = s * 10 + (ord(row[i]) - 48)
            i += 1
        # scan flag
        while i < n and row[i] != "f": i += 1
        i += 5  # skip "flag="
        while i < n and row[i].isdigit():
            f = f * 10 + (ord(row[i]) - 48)
            i += 1
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h