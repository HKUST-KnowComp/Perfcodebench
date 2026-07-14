import json


_MASK64 = (1 << 64) - 1
_OFFSET = 1469598103934665603
_PRIME = 1099511628211


def _parse_int_after_colon(s: str, i: int) -> tuple[int, int]:
    n = len(s)
    while i < n:
        c = s[i]
        if c != ' ' and c != '\t' and c != '\r' and c != '\n':
            break
        i += 1
    sign = 1
    if i < n and s[i] == '-':
        sign = -1
        i += 1
    v = 0
    while i < n:
        o = ord(s[i]) - 48
        if 0 <= o <= 9:
            v = v * 10 + o
            i += 1
        else:
            break
    return sign * v, i


def _parse_bool_after_colon(s: str, i: int) -> bool:
    n = len(s)
    while i < n:
        c = s[i]
        if c != ' ' and c != '\t' and c != '\r' and c != '\n':
            break
        i += 1
    return i < n and s[i] == 't'


def _extract_fields(row: str):
    iu = row.find('"u"')
    if iu < 0:
        return None
    iu = row.find(':', iu + 3)
    if iu < 0:
        return None
    u, _ = _parse_int_after_colon(row, iu + 1)

    is_ = row.find('"s"')
    if is_ < 0:
        return None
    is_ = row.find(':', is_ + 3)
    if is_ < 0:
        return None
    sval, _ = _parse_int_after_colon(row, is_ + 1)

    iff = row.find('"f"')
    if iff < 0:
        return None
    iff = row.find(':', iff + 3)
    if iff < 0:
        return None
    fval = _parse_bool_after_colon(row, iff + 1)

    return u, sval, fval


def run(rows: list[str]) -> int:
    h = _OFFSET
    mask = _MASK64
    prime = _PRIME
    loads = json.loads
    extract = _extract_fields

    for row in rows:
        vals = extract(row)
        if vals is None:
            obj = loads(row)
            v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        else:
            u, s, f = vals
            v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h = (h * prime) & mask
    return h
