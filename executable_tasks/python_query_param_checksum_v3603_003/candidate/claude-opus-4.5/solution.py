def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for row in rows:
        # Find u=
        ui = row.find('u=')
        ue = row.find('&', ui)
        if ue == -1:
            u = int(row[ui + 2:])
        else:
            u = int(row[ui + 2:ue])
        
        # Find score=
        si = row.find('score=')
        se = row.find('&', si)
        if se == -1:
            s = int(row[si + 6:])
        else:
            s = int(row[si + 6:se])
        
        # Find flag=
        fi = row.find('flag=')
        fe = row.find('&', fi)
        if fe == -1:
            f = int(row[fi + 5:])
        else:
            f = int(row[fi + 5:fe])
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
