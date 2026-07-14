def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        # Parse 'u'
        idx = row.find('u=')
        if idx == -1:
            raise KeyError('u')
        start = idx + 2
        end = row.find('&', start)
        u_str = row[start:] if end == -1 else row[start:end]
        u = int(u_str)
        
        # Parse 'score'
        idx = row.find('score=')
        if idx == -1:
            raise KeyError('score')
        start = idx + 6
        end = row.find('&', start)
        s_str = row[start:] if end == -1 else row[start:end]
        s = int(s_str)
        
        # Parse 'flag'
        idx = row.find('flag=')
        if idx == -1:
            raise KeyError('flag')
        start = idx + 5
        end = row.find('&', start)
        f_str = row[start:] if end == -1 else row[start:end]
        f = int(f_str)
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
