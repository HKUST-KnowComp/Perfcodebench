from typing import List

def run(rows: List[str]) -> int:
    h = 14695981039346656037
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        # scan u
        while i < n and row[i] != '=': i += 1
        i += 1
        while i < n and row[i].isdigit():
            u = u * 10 + (ord(row[i]) - 48)
            i += 1
        # scan score
        while i < n and row[i] != '=': i += 1
        i += 1
        while i < n and row[i].isdigit():
            s = s * 10 + (ord(row[i]) - 48)
            i += 1
        # scan flag
        while i < n and row[i] != '=': i += 1
        i += 1
        while i < n and row[i].isdigit():
            f = f * 10 + (ord(row[i]) - 48)
            i += 1
        v = (u & 2047) + s + f * 19
        h ^= (v + 1) & 0xFFFFFFFFFFFFFFFF
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h