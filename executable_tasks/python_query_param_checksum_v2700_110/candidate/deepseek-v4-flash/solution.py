from urllib.parse import parse_qs


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    for row in rows:
        u = s = f = None
        i = 0
        n = len(row)
        while i < n:
            # find next '&' or end of string
            amp = row.find('&', i)
            if amp == -1:
                amp = n
            # find '=' in segment [i, amp)
            eq = row.find('=', i, amp)
            if eq != -1:
                key = row[i:eq]
                val = row[eq+1:amp]
                # extract only the required keys; ignore others
                if key == 'u':
                    u = int(val)
                elif key == 'score':
                    s = int(val)
                elif key == 'flag':
                    f = int(val)
            # move past the '&' (or end)
            i = amp + 1
        # compute checksum contribution for this row
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= MASK
    return h