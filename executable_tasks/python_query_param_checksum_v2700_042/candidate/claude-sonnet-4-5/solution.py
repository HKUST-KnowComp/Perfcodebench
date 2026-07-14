from urllib.parse import parse_qs


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Manual parsing: find positions of '=' after each key
        u_pos = row.find('u=') + 2
        u_end = row.find('&', u_pos)
        u = int(row[u_pos:u_end])
        
        score_pos = row.find('score=') + 6
        score_end = row.find('&', score_pos)
        s = int(row[score_pos:score_end])
        
        flag_pos = row.find('flag=') + 5
        flag_end = row.find('&', flag_pos)
        if flag_end == -1:
            f = int(row[flag_pos:])
        else:
            f = int(row[flag_pos:flag_end])
        
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
