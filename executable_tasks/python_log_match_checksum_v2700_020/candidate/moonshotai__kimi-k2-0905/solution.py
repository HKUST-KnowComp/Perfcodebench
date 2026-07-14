import string

_HEX = set(string.hexdigits)          # 0-9A-Fa-f


def run(rows: list[str]) -> int:
    h = 14695981039346656037            # FNV offset basis (64-bit)
    for row in rows:
        # fast path: exact length and fixed-position character checks
        if len(row) == 14 and row[2].isdigit() and row[7].isdigit() and row[4] == '-':
            # check upper-case letters at 0,1
            c0, c1 = row[0], row[1]
            if c0.isupper() and c1.isupper():
                # check 8 hex digits after the hyphen
                if all(ch in _HEX for ch in row[5:14]):
                    v = (ord(row[2]) + ord(row[7]))
                    h ^= (v + 1)
                    h *= 1099511628211
                    h &= 0xFFFFFFFFFFFFFFFF
                    continue
        # no match: xor 0, multiply, mask
        h ^= 1
        h *= 1099511628211
        h &= 0xFFFFFFFFFFFFFFFF
    return h