from typing import List

_MASK64 = (1 << 64) - 1


def _parse_int(s: str, pos: int) -> tuple[int, int]:
    """Parse an unsigned decimal integer starting at pos; return (value, next_pos)."""
    v = 0
    while pos < len(s) and s[pos].isdigit():
        v = v * 10 + (ord(s[pos]) - 48)
        pos += 1
    return v, pos


def run(rows: List[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        u = s = f = 0
        i = 0
        n = len(row)
        while i < n:
            # locate key start
            key_start = i
            while i < n and s[i] != "=" and s[i] != "&":
                i += 1
            key = row[key_start:i]
            if i < n and row[i] == "=":
                i += 1  # skip '='
                val_start = i
                while i < n and row[i] != "&":
                    i += 1
                val = int(row[val_start:i])
                if key == "u":
                    u = val
                elif key == "score":
                    s = val
                elif key == "flag":
                    f = val
            if i < n and row[i] == "&":
                i += 1
        v = (u & 2047) + s + f * 19
        h ^= (v + 1)
        h = (h * 1099511628211) & _MASK64
    return h