def unquote(s: str) -> str:
    if '%' not in s:
        return s
    res = []
    i = 0
    n = len(s)
    while i < n:
        c = s[i]
        if c == '%' and i + 2 < n:
            hi = s[i+1]
            lo = s[i+2]
            if '0' <= hi <= '9':
                code = ord(hi) - 48
            elif 'a' <= hi <= 'f':
                code = ord(hi) - 87
            elif 'A' <= hi <= 'F':
                code = ord(hi) - 55
            else:
                code = -1
            if code != -1:
                if '0' <= lo <= '9':
                    code = (code << 4) | (ord(lo) - 48)
                elif 'a' <= lo <= 'f':
                    code = (code << 4) | (ord(lo) - 87)
                elif 'A' <= lo <= 'F':
                    code = (code << 4) | (ord(lo) - 55)
                else:
                    code = -1
            if code != -1:
                res.append(chr(code))
                i += 3
                continue
        res.append(c)
        i += 1
    return ''.join(res)


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = None
        s = None
        f = None
        for param in row.split('&'):
            if '=' in param:
                key, val = param.split('=', 1)
                if not val:
                    continue
                if key == 'u':
                    u = int(unquote(val))
                elif key == 'score':
                    s = int(unquote(val))
                elif key == 'flag':
                    flag_val = int(unquote(val))
        v = (u & 2047) + s + flag_val * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h