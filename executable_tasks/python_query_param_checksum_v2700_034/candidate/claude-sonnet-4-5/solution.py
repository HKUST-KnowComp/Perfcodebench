from urllib.parse import parse_qs


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Manual parsing - find each parameter directly
        u_idx = row.find('u=')
        u_end = row.find('&', u_idx)
        u = int(row[u_idx+2:u_end] if u_end != -1 else row[u_idx+2:])
        
        s_idx = row.find('score=')
        s_end = row.find('&', s_idx)
        s = int(row[s_idx+6:s_end] if s_end != -1 else row[s_idx+6:])
        
        f_idx = row.find('flag=')
        f_end = row.find('&', f_idx)
        f = int(row[f_idx+5:f_end] if f_end != -1 else row[f_idx+5:])
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
