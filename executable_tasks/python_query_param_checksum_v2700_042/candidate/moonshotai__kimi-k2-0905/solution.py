from typing import List

def run(rows: List[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        # parse_qs equivalent: find u, score, flag in any order
        while i < n:
            # find next key start
            while i < n and row[i] != '&':
                i += 1
            if i < n:
                i += 1  # skip '&'
            start = i
            # find '='
            while i < n and row[i] != '=':
                i += 1
            if i == n:
                break
            key = row[start:i]
            i += 1  # skip '='
            # find value end
            val_start = i
            while i < n and row[i] != '&':
                i += 1
            val = row[val_start:i]
            # parse value
            if key == "u":
                u = int(val)
            elif key == "score":
                s = int(val)
            elif key == "flag":
                f = int(val)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h