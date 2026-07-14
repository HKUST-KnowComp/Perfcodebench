import re


# Pre-compiled regex for each parameter.  The pattern grabs the first decimal
# value that immediately follows the key plus '='.
_u_re   = re.compile(br'[&?]u=(\d+)')
_score_re = re.compile(br'[&?]score=(\d+)')
_flag_re  = re.compile(br'[&?]flag=(\d+)')


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Work with bytes; this is faster than UTF-8 decoding in CPython
        b = row.encode('ascii')

        u   = int(_u_re.search(b).group(1))
        s   = int(_score_re.search(b).group(1))
        f   = int(_flag_re.search(b).group(1))

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h