def run(rows):
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    multiplier = 1099511628211
    ord_ = ord  # local lookup
    for row in rows:
        v = 0
        if len(row) == 15:
            if 'A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z':
                if '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9':
                    if row[6] == '-':
                        # check 8 hex digits
                        hex_ok = True
                        for i in range(7, 15):
                            c = row[i]
                            if not ('0' <= c <= '9' or 'A' <= c <= 'F'):
                                hex_ok = False
                                break
                        if hex_ok:
                            v = ord_(row[2]) + ord_(row[7])
        h ^= (v + 1)
        h = (h * multiplier) & mask
    return h
