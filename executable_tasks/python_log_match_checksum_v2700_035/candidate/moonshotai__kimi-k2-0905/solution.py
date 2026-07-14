def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # fast fixed-pattern check: AA0000-00000000
        if (len(row) == 13 and
            row[0] >= 'A' and row[0] <= 'Z' and
            row[1] >= 'A' and row[1] <= 'Z' and
            row[2] >= '0' and row[2] <= '9' and
            row[3] >= '0' and row[3] <= '9' and
            row[4] >= '0' and row[4] <= '9' and
            row[5] >= '0' and row[5] <= '9' and
            row[6] == '-' and
            ((row[7]  >= '0' and row[7]  <= '9') or (row[7]  >= 'A' and row[7]  <= 'F')) and
            ((row[8]  >= '0' and row[8]  <= '9') or (row[8]  >= 'A' and row[8]  <= 'F')) and
            ((row[9]  >= '0' and row[9]  <= '9') or (row[9]  >= 'A' and row[9]  <= 'F')) and
            ((row[10] >= '0' and row[10] <= '9') or (row[10] >= 'A' and row[10] <= 'F')) and
            ((row[11] >= '0' and row[11] <= '9') or (row[11] >= 'A' and row[11] <= 'F')) and
            ((row[12] >= '0' and row[12] <= '9') or (row[12] >= 'A' and row[12] <= 'F'))):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h