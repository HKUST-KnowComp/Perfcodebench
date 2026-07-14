def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    prime = 1099511628211
    
    for row in rows:
        # Find 'u=' parameter
        i = row.find('u=')
        if i >= 0:
            j = i + 2
            k = row.find('&', j)
            if k < 0:
                u = int(row[j:])
            else:
                u = int(row[j:k])
        else:
            u = 0
        
        # Find 'score=' parameter
        i = row.find('score=')
        if i >= 0:
            j = i + 6
            k = row.find('&', j)
            if k < 0:
                s = int(row[j:])
            else:
                s = int(row[j:k])
        else:
            s = 0
        
        # Find 'flag=' parameter
        i = row.find('flag=')
        if i >= 0:
            j = i + 5
            k = row.find('&', j)
            if k < 0:
                f = int(row[j:])
            else:
                f = int(row[j:k])
        else:
            f = 0
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= prime
        h &= mask
    
    return h
