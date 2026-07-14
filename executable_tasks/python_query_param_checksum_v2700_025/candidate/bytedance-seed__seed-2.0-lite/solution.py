def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    for row in rows:
        # Extract u value
        u_pos = row.find("u=")
        u_start = u_pos + 2
        u_end = row.find('&', u_start)
        u = int(row[u_start:u_end]) if u_end != -1 else int(row[u_start:])
        
        # Extract score value
        s_pos = row.find("score=")
        s_start = s_pos + 6
        s_end = row.find('&', s_start)
        s = int(row[s_start:s_end]) if s_end != -1 else int(row[s_start:])
        
        # Extract flag value
        f_pos = row.find("flag=")
        f_start = f_pos + 5
        f_end = row.find('&', f_start)
        f = int(row[f_start:f_end]) if f_end != -1 else int(row[f_start:])
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask64
    return h