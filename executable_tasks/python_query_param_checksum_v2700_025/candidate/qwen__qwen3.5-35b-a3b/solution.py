from urllib.parse import unquote

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        params = {}
        parts = row.split('&')
        for part in parts:
            if '=' in part:
                key, value = part.split('=', 1)
                if value == '':
                    continue
                key = unquote(key)
                value = unquote(value)
                if key not in params:
                    params[key] = value
        u = int(params["u"])
        s = int(params["score"])
        f = int(params["flag"])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h