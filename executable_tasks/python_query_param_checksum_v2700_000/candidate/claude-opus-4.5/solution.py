def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Find u=
        idx = row.find('u=')
        if idx != -1:
            idx += 2
            n = len(row)
            while idx < n and row[idx].isdigit():
                u = u * 10 + (ord(row[idx]) - 48)
                idx += 1
        
        # Find score=
        idx = row.find('score=')
        if idx != -1:
            idx += 6
            n = len(row)
            while idx < n and row[idx].isdigit():
                s = s * 10 + (ord(row[idx]) - 48)
                idx += 1
        
        # Find flag=
        idx = row.find('flag=')
        if idx != -1:
            idx += 5
            n = len(row)
            while idx < n and row[idx].isdigit():
                f = f * 10 + (ord(row[idx]) - 48)
                idx += 1
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
