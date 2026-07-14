def run(rows: list[str]) -> int:
    OFFSET = 1469598103934665603
    PRIME = 1099511628211
    MASK = (1 << 64) - 1
    h = OFFSET
    for row in rows:
        # Parse u
        idx = row.find('"u"')
        start = row.index(':', idx + 3) + 1
        while row[start] == ' ':
            start += 1
        sign = 1
        if row[start] == '-':
            sign = -1
            start += 1
        end = start
        while end < len(row) and row[end].isdigit():
            end += 1
        u = int(row[start:end]) * sign

        # Parse s
        idx = row.find('"s"')
        start = row.index(':', idx + 3) + 1
        while row[start] == ' ':
            start += 1
        sign = 1
        if row[start] == '-':
            sign = -1
            start += 1
        end = start
        while end < len(row) and row[end].isdigit():
            end += 1
        s = int(row[start:end]) * sign

        # Parse f
        idx = row.find('"f"')
        start = row.index(':', idx + 3) + 1
        while row[start] == ' ':
            start += 1
        f = row.startswith('true', start)

        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h = (h * PRIME) & MASK
    return h