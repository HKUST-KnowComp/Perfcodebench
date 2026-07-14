from typing import List

def run(rows: List[str]) -> int:
    h = 14695981039346656037
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        while i < n:
            if row[i] == "u" and row[i+1] == "=":
                i += 2
                v = 0
                while i < n and row[i].isdigit():
                    v = v * 10 + (ord(row[i]) - 48)
                    i += 1
                u = v
            elif row[i] == "s" and i + 5 < n and row[i:i+6] == "score=":
                i += 6
                v = 0
                while i < n and row[i].isdigit():
                    v = v * 10 + (ord(row[i]) - 48)
                    i += 1
                s = v
            elif row[i] == "f" and i + 3 < n and row[i:i+5] == "flag=":
                i += 5
                v = 0
                while i < n and row[i].isdigit():
                    v = v * 10 + (ord(row[i]) - 48)
                    i += 1
                f = v
            else:
                i += 1
        val = (u & 2047) + s + f * 19
        h ^= (val + 1) & 0xFFFFFFFFFFFFFFFF
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h