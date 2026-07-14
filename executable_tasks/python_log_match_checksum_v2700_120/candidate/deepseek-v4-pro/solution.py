def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211

    for row in rows:
        v = 0
        if len(row) == 15:
            if ('A' <= row[0] <= 'Z' and 'A' <= row[1] <= 'Z' and
                '0' <= row[2] <= '9' and '0' <= row[3] <= '9' and '0' <= row[4] <= '9' and '0' <= row[5] <= '9' and
                row[6] == '-' and
                (('0' <= row[7] <= '9') or ('A' <= row[7] <= 'F')) and
                (('0' <= row[8] <= '9') or ('A' <= row[8] <= 'F')) and
                (('0' <= row[9] <= '9') or ('A' <= row[9] <= 'F')) and
                (('0' <= row[10] <= '9') or ('A' <= row[10] <= 'F')) and
                (('0' <= row[11] <= '9') or ('A' <= row[11] <= 'F')) and
                (('0' <= row[12] <= '9') or ('A' <= row[12] <= 'F')) and
                (('0' <= row[13] <= '9') or ('A' <= row[13] <= 'F')) and
                (('0' <= row[14] <= '9') or ('A' <= row[14] <= 'F'))):
                v = ord(row[2]) + ord(row[7])

        h ^= v + 1
        h *= mul
        h &= mask

    return h
