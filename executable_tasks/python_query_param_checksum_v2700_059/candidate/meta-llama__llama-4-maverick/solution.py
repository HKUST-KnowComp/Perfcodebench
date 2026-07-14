```python
from urllib.parse import parse_qsl

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        q = dict(parse_qsl(row, keep_blank_values=False, strict_parsing=False))
        u = int(q["u"])
        s = int(q["score"])
        f = int(q["flag"])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
```