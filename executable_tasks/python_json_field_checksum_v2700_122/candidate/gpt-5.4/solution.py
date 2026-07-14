import json

_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


# Fast parser specialized for task rows containing keys: "u", "s", "f".
# Falls back to json.loads for any unexpected formatting/content.
def _extract_v(row: str):
    n = len(row)
    i = 0
    u = None
    s = None
    f = None

    while i < n:
        if row[i] != '"':
            i += 1
            continue

        j = i + 1
        while j < n and row[j] != '"':
            # Fallback on escaped strings; generic JSON parser handles them.
            if row[j] == '\\':
                return None
            j += 1
        if j >= n:
            return None

        key = row[i + 1:j]
        k = j + 1
        while k < n and row[k] in ' \t\r\n':
            k += 1
        if k >= n or row[k] != ':':
            i = j + 1
            continue
        k += 1
        while k < n and row[k] in ' \t\r\n':
            k += 1
        if k >= n:
            return None

        if key == 'u' or key == 's':
            sign = 1
            if row[k] == '-':
                sign = -1
                k += 1
                if k >= n:
                    return None
            if k >= n or not ('0' <= row[k] <= '9'):
                return None
            val = 0
            while k < n:
                c = row[k]
                if '0' <= c <= '9':
                    val = val * 10 + (ord(c) - 48)
                    k += 1
                else:
                    break
            val *= sign
            if key == 'u':
                u = val
            else:
                s = val
            i = k
            continue

        if key == 'f':
            c = row[k]
            if c == 't':
                if row.startswith('true', k):
                    f = True
                    i = k + 4
                    continue
                return None
            if c == 'f':
                if row.startswith('false', k):
                    f = False
                    i = k + 5
                    continue
                return None
            return None

        # Skip unknown value conservatively; fallback for complex cases.
        c = row[k]
        if c == '"' or c == '{' or c == '[':
            return None
        while k < n and row[k] not in ',}':
            k += 1
        i = k

    if u is None or s is None or f is None:
        return None
    return (u & 1023) + s + (17 if f else 0)


def run(rows: list[str]) -> int:
    h = _FNV_OFFSET
    for row in rows:
        v = _extract_v(row)
        if v is None:
            obj = json.loads(row)
            v = (obj['u'] & 1023) + obj['s'] + (17 if obj['f'] else 0)
        h ^= v + 1
        h = (h * _FNV_PRIME) & _MASK64
    return h
