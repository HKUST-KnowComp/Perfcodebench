from typing import List

def run(rows: List[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        while i < n:
            if row[i] == 'u' and row[i+1] == '=':
                i += 2
                v = 0
                while i < n and row[i].isdigit():
                    v = v * 10 + (ord(row[i]) - 48)
                    i += 1
                u = v
            elif row[i] == 's' and row[i+1] == 'c' and row[i+2] == 'o' and row[i+3] == 'r' and row[i+4] == 'e' and row[i+5] == '=':
                i += 6
                v = 0
                while i < n and row[i].isdigit():
                    v = v * 10 + (ord(row[i]) - 48)
                    i += 1
                s = v
            elif row[i] == 'f' and row[i+1] == 'l' and row[i+2] == 'a' and row[i+3] == 'g' and row[i+4] == '=':
                i += 5
                v = 0
                while i < n and row[i].isdigit():
                    v = v * 10 + (ord(row[i]) - 48)
                    i += 1
                f = v
            else:
                i += 1
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h