def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    
    for row in rows:
        # Find u parameter
        ui = row.find('u=')
        if ui != -1:
            ui += 2
            ue = row.find('&', ui)
            if ue == -1:
                u = int(row[ui:])
            else:
                u = int(row[ui:ue])
        else:
            u = 0
        
        # Find score parameter
        si = row.find('score=')
        if si != -1:
            si += 6
            se = row.find('&', si)
            if se == -1:
                s = int(row[si:])
            else:
                s = int(row[si:se])
        else:
            s = 0
        
        # Find flag parameter
        fi = row.find('flag=')
        if fi != -1:
            fi += 5
            fe = row.find('&', fi)
            if fe == -1:
                f = int(row[fi:])
            else:
                f = int(row[fi:fe])
        else:
            f = 0
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * MULT) & MASK
    
    return h
