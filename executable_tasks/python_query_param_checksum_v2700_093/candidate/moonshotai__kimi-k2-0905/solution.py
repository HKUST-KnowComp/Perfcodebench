from typing import List


def run(rows: List[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1

    for row in rows:
        u = score = flag = None
        i = 0
        n = len(row)

        while i < n:
            # Locate next key=value segment
            amp = row.find('&', i)
            if amp == -1:
                amp = n
            eq = row.find('=', i, amp)
            if eq == -1:
                i = amp + 1
                continue

            key_start = i
            key_end = eq
            val_start = eq + 1
            val_end = amp

            # Check key length quickly to avoid slice/compare for non-matches
            key_len = key_end - key_start
            if key_len == 1 and row[key_start] == 'u':
                u = int(row[val_start:val_end])
            elif key_len == 5 and row[key_start:key_start+5] == 'score':
                score = int(row[val_start:val_end])
            elif key_len == 4 and row[key_start:key_start+4] == 'flag':
                flag = int(row[val_start:val_end])
            i = amp + 1

        v = (u & 2047) + score + flag * 19
        h ^= v + 1
        h *= 1099511628211
        h &= mask

    return h