def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    
    for row in rows:
        # Find u parameter
        idx = row.find('u=')
        if idx != -1:
            start = idx + 2
            end = row.find('&', start)
            if end == -1:
                u = int(row[start:])
            else:
                u = int(row[start:end])
        else:
            u = 0
        
        # Find score parameter
        idx = row.find('score=')
        if idx != -1:
            start = idx + 6
            end = row.find('&', start)
            if end == -1:
                s = int(row[start:])
            else:
                s = int(row[start:end])
        else:
            s = 0
        
        # Find flag parameter
        idx = row.find('flag=')
        if idx != -1:
            start = idx + 5
            end = row.find('&', start)
            if end == -1:
                f = int(row[start:])
            else:
                f = int(row[start:end])
        else:
            f = 0
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * mult) & mask
    
    return h
