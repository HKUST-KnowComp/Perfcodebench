def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for row in rows:
        u = s = f = 0
        if row.startswith('?'):
            row = row[1:]
        for part in row.split('&'):
            if part.startswith('u='):
                u = int(part[2:])
            elif part.startswith('score='):
                s = int(part[6:])
            elif part.startswith('flag='):
                f = int(part[5:])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= prime
        h &= mask
    return h