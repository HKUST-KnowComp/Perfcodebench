import sys

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    for row in rows:
        # find "u="
        pos = row.find('u=')
        assert pos != -1
        start = pos + 2
        pos = row.find('&', start)
        if pos == -1:
            u = int(row[start:])
            end = len(row)
        else:
            u = int(row[start:pos])
            end = pos

        # find "score="
        pos = row.find('score=', end)
        assert pos != -1
        start = pos + 6
        pos = row.find('&', start)
        if pos == -1:
            score = int(row[start:])
            end = len(row)
        else:
            score = int(row[start:pos])
            end = pos

        # find "flag="
        pos = row.find('flag=', end)
        assert pos != -1
        start = pos + 5
        flag = int(row[start:])

        v = (u & 2047) + score + flag * 19
        h ^= v + 1
        h *= 1099511628211
        h &= MASK
    return h