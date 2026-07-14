import re

# The JSON we expect looks like:
# {"u":<int>,"s":<int>,"f":<true|false>}
# or permutations of the keys.
#
# We will scan each line once, extract the three values, and compute the hash.

_DIGIT = {b'0'[0]: 0, b'1'[0]: 1, b'2'[0]: 2, b'3'[0]: 3, b'4'[0]: 4,
          b'5'[0]: 5, b'6'[0]: 6, b'7'[0]: 7, b'8'[0]: 8, b'9'[0]: 9}
_TRUE = b'true'
_FALSE = b'false'


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Work on bytes for speed
        b = row.encode('utf-8')  # ASCII subset is fine
        n = len(b)
        i = 0

        u = s = None
        f = None

        # Scan to find the three keys and their values
        while i < n:
            # Look for '"' to start a key
            if b[i] == ord('"'):
                i += 1
                # Minimum key size is 1 char + quote
                if i + 1 >= n:
                    break
                key_start = i
                # Find closing quote
                while i < n and b[i] != ord('"'):
                    i += 1
                if i >= n:
                    break
                key = b[key_start:i]
                i += 1  # skip closing quote

                # Expect colon
                while i < n and b[i] in (ord(' '), ord('\t'), ord('\n'), ord('\r')):
                    i += 1
                if i >= n or b[i] != ord(':'):
                    break
                i += 1
                while i < n and b[i] in (ord(' '), ord('\t'), ord('\n'), ord('\r')):
                    i += 1

                # Parse value
                if key == b'u' and u is None:
                    # Parse integer
                    negative = False
                    if b[i] == ord('-'):
                        negative = True
                        i += 1
                    val = 0
                    while i < n and b[i] >= ord('0') and b[i] <= ord('9'):
                        val = val * 10 + _DIGIT[b[i]]
                        i += 1
                    if negative:
                        val = -val
                    u = val
                elif key == b's' and s is None:
                    # Parse integer
                    negative = False
                    if b[i] == ord('-'):
                        negative = True
                        i += 1
                    val = 0
                    while i < n and b[i] >= ord('0') and b[i] <= ord('9'):
                        val = val * 10 + _DIGIT[b[i]]
                        i += 1
                    if negative:
                        val = -val
                    s = val
                elif key == b'f' and f is None:
                    # Parse boolean
                    if b[i] == ord('t'):
                        # true
                        if i + 4 <= n and b[i:i+4] == _TRUE:
                            f = True
                            i += 4
                    elif b[i] == ord('f'):
                        # false
                        if i + 5 <= n and b[i:i+5] == _FALSE:
                            f = False
                            i += 5
                else:
                    # Skip value
                    if b[i] == ord('"'):
                        i += 1
                        while i < n and b[i] != ord('"'):
                            i += 1
                        if i < n:
                            i += 1
                    elif b[i] == ord('t') or b[i] == ord('f'):
                        if b[i] == ord('t') and i + 4 <= n and b[i:i+4] == _TRUE:
                            i += 4
                        elif b[i] == ord('f') and i + 5 <= n and b[i:i+5] == _FALSE:
                            i += 5
                    elif b[i] == ord('n') and i + 4 <= n and b[i:i+4] == b'null':
                        i += 4
                    elif b[i] == ord('-') or (b[i] >= ord('0') and b[i] <= ord('9')):
                        if b[i] == ord('-'):
                            i += 1
                        while i < n and (b[i] >= ord('0') and b[i] <= ord('9')):
                            i += 1
            else:
                i += 1

        # Compute v and hash
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1

    return h