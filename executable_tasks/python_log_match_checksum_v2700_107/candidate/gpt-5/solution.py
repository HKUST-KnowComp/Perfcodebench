def run(rows: list[str]) -> int:
    # FNV-1 like 64-bit hash parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        v = 0
        # Fast path: manual validation for pattern ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
        if len(row) == 15:
            c0 = row[0]; c1 = row[1]
            # First two uppercase A-Z
            if 'A' <= c0 <= 'Z' and 'A' <= c1 <= 'Z':
                c2 = row[2]; c3 = row[3]; c4 = row[4]; c5 = row[5]
                # Next four digits 0-9
                if '0' <= c2 <= '9' and '0' <= c3 <= '9' and '0' <= c4 <= '9' and '0' <= c5 <= '9':
                    # Hyphen
                    if row[6] == '-':
                        c7 = row[7]; c8 = row[8]; c9 = row[9]; c10 = row[10]
                        c11 = row[11]; c12 = row[12]; c13 = row[13]; c14 = row[14]
                        # Hex uppercase [0-9A-F] for eight chars
                        if ((('0' <= c7 <= '9') or ('A' <= c7 <= 'F')) and
                            (('0' <= c8 <= '9') or ('A' <= c8 <= 'F')) and
                            (('0' <= c9 <= '9') or ('A' <= c9 <= 'F')) and
                            (('0' <= c10 <= '9') or ('A' <= c10 <= 'F')) and
                            (('0' <= c11 <= '9') or ('A' <= c11 <= 'F')) and
                            (('0' <= c12 <= '9') or ('A' <= c12 <= 'F')) and
                            (('0' <= c13 <= '9') or ('A' <= c13 <= 'F')) and
                            (('0' <= c14 <= '9') or ('A' <= c14 <= 'F'))):
                            v = ord(c2) + ord(c7)
        h ^= v + 1
        h *= prime
        h &= mask
    return h
