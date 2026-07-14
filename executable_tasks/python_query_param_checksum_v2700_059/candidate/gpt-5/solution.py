def _parse_int_substr(s: str, start: int, end: int) -> int:
    if start >= end:
        # Mimic int("") behavior (which would raise); inputs are expected valid
        raise ValueError("empty integer")
    i = start
    neg = False
    c = s[i]
    if c == '-':
        neg = True
        i += 1
    elif c == '+':
        i += 1
    if i >= end:
        raise ValueError("invalid integer")
    val = 0
    # parse digits
    while i < end:
        ch = s[i]
        d = ord(ch) - 48
        if d < 0 or d > 9:
            raise ValueError("invalid digit in integer")
        val = val * 10 + d
        i += 1
    return -val if neg else val


def run(rows: list[str]) -> int:
    mask = (1 << 64) - 1
    h = 1469598103934665603
    for row in rows:
        n = len(row)
        i = 0
        u = None
        sc = None
        fl = None
        while i < n:
            # parse key
            kstart = i
            # find '=' or '&'
            while i < n and row[i] != '=' and row[i] != '&':
                i += 1
            if i >= n or row[i] != '=':
                # no '=' found for this segment; skip to next after '&'
                while i < n and row[i] != '&':
                    i += 1
                if i < n and row[i] == '&':
                    i += 1
                continue
            kend = i
            i += 1  # skip '='
            vstart = i
            # find end of value
            while i < n and row[i] != '&':
                i += 1
            vend = i

            key_len = kend - kstart
            if key_len == 1:
                ch = row[kstart]
                if ch == 'u' and u is None:
                    u = _parse_int_substr(row, vstart, vend)
            elif key_len == 5:
                # "score"
                if (
                    row[kstart] == 's' and row[kstart + 1] == 'c' and row[kstart + 2] == 'o'
                    and row[kstart + 3] == 'r' and row[kstart + 4] == 'e' and sc is None
                ):
                    sc = _parse_int_substr(row, vstart, vend)
            elif key_len == 4:
                # "flag"
                if (
                    row[kstart] == 'f' and row[kstart + 1] == 'l' and row[kstart + 2] == 'a'
                    and row[kstart + 3] == 'g' and fl is None
                ):
                    fl = _parse_int_substr(row, vstart, vend)

            # move past '&' if present
            if i < n and row[i] == '&':
                i += 1

            if u is not None and sc is not None and fl is not None:
                break

        # Expect required keys to be present as per task definition
        # Compute checksum contribution
        v = (u & 2047) + sc + fl * 19  # type: ignore[operator]
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h
