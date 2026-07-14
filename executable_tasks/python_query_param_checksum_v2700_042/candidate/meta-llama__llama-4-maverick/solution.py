from urllib.parse import parse_qs

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Manually parse query string to reduce function call overhead
        q = {}
        for pair in row.split('&'):
            if '=' in pair:
                k, v = pair.split('=', 1)
                if k not in q:
                    q[k] = [v]
                else:
                    q[k].append(v)
        # Directly parse integers from the query values without extra checks
        u = int(q['u'][0])
        s = int(q['score'][0])
        f = int(q['flag'][0])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h