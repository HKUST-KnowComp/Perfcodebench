def run(rows: list[str]) -> int:
    h = 1469598103934665603
    find = str.find
    int_ = int
    key_u = 'u='
    key_score = 'score='
    key_flag = 'flag='
    for row in rows:
        # extract u
        idx = find(row, key_u)
        if idx == -1:
            raise ValueError('Missing u')
        start = idx + 2
        end = find(row, '&', start)
        if end == -1:
            end = len(row)
        u = int_(row[start:end])

        # extract score
        idx = find(row, key_score)
        if idx == -1:
            raise ValueError('Missing score')
        start = idx + 6
        end = find(row, '&', start)
        if end == -1:
            end = len(row)
        s = int_(row[start:end])

        # extract flag
        idx = find(row, key_flag)
        if idx == -1:
            raise ValueError('Missing flag')
        start = idx + 5
        end = find(row, '&', start)
        if end == -1:
            end = len(row)
        f = int_(row[start:end])

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
