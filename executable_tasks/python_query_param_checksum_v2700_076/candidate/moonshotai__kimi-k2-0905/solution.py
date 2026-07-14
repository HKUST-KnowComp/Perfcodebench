from typing import List

# 2048-byte perfect hash for fast char-classification
_CHAR_MAP = bytearray(256)
for _i in range(256):
    _CHAR_MAP[_i] = 0
_CHAR_MAP[ord('=')] = 1
_CHAR_MAP[ord('&')] = 2

# bit-masks for quick equality tests
_EQ  = 1
_AMP = 2

def run(rows: List[str]) -> int:
    h = 1469598103934665603
    rows_length = len(rows)
    i = 0
    # four-way unroll to hide interpreter overhead
    while i + 3 < rows_length:
        h = _proc_row(rows[i], h)
        h = _proc_row(rows[i+1], h)
        h = _proc_row(rows[i+2], h)
        h = _proc_row(rows[i+3], h)
        i += 4
    # tail
    while i < rows_length:
        h = _proc_row(rows[i], h)
        i += 1
    return h

# ---- internal hot helper ----
def _proc_row(row: str, h: int) -> int:
    # scan once, pick out u, score, flag manually
    u = 0
    score = 0
    flag  = 0
    state = 0  # 0=seekName, 1=u, 2=score, 3=flag, 4=value
    val = 0
    for ch in row:
        m = _CHAR_MAP[ord(ch)]
        if m & _EQ:           # found '='
            state = val
            val = 0
            continue
        if m & _AMP:          # found '&'
            if state == 1: u = val
            elif state == 2: score = val
            elif state == 3: flag = val
            state = 0
            val = 0
            continue
        # digit accumulation
        chint = ord(ch)
        if 48 <= chint <= 57:
            val = val*10 + (chint - 48)
        else:
            # handle name matching: bump state on exact match start
            if ch == 'u' and state == 0:
                state = 1
                continue
            if ch == 's':     # either 'score' or 's' prefix
                if state == 0 and row[val:val+5] == 'score':
                    state = 2
                    val += 4
                    continue
            if ch == 'f':     # either 'flag' or 'f' prefix
                if state == 0 and row[val:val+4] == 'flag':
                    state = 3
                    val += 3
                    continue
            state = 0
    # final value flush
    if state == 1: u = val
    elif state == 2: score = val
    elif state == 3: flag = val

    # original formula
    v = (u & 2047) + score + flag * 19
    h ^= v + 1
    h *= 1099511628211
    return h & ((1 << 64) - 1)