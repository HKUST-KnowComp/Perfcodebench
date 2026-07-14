import json
def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for row in rows:
        u = _parse_int_after_key(row, 'u')
        s = _parse_int_after_key(row, 's')
        f = _parse_bool_after_key(row, 'f')
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h = (h * mult) & mask
    return h

def _parse_int_after_key(s: str, key: str) -> int:
    key_str = f'"{key}":'
    idx = s.find(key_str)
    if idx == -1:
        raise ValueError(f"Key '{key}' not found")
    pos = idx + len(key_str)
    # skip whitespace
    while pos < len(s) and s[pos] in ' \t\n\r':
        pos += 1
    sign = 1
    if pos < len(s) and s[pos] == '-':
        sign = -1
        pos += 1
    num = 0
    while pos < len(s) and '0' <= s[pos] <= '9':
        num = num * 10 + (ord(s[pos]) - 48)
        pos += 1
    return sign * num

def _parse_bool_after_key(s: str, key: str) -> bool:
    key_str = f'"{key}":'
    idx = s.find(key_str)
    if idx == -1:
        raise ValueError(f"Key '{key}' not found")
    pos = idx + len(key_str)
    while pos < len(s) and s[pos] in ' \t\n\r':
        pos += 1
    if s[pos:pos+4] == 'true':
        return True
    elif s[pos:pos+5] == 'false':
        return False
    raise ValueError(f"Boolean value expected after key '{key}'")

__all__ = ['run']
