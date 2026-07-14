def run(rows: list[str]) -> int:
    # FNV-1a inspired hash constants
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mult = 1099511628211

    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual parsing is significantly faster than urllib.parse.parse_qs
        # for simple key-value pairs in a performance-critical loop.
        for part in row.split('&'):
            if not part:
                continue
            if part.startswith('u='):
                u = int(part[2:])
            elif part.startswith('score='):
                s = int(part[6:])
            elif part.startswith('flag='):
                f = int(part[5:])
        
        # Calculation logic preserved from baseline
        v = (u & 2047) + s + f * 19
        
        # Hash update logic preserved from baseline
        h ^= (v + 1)
        h = (h * mult) & mask
        
    return h