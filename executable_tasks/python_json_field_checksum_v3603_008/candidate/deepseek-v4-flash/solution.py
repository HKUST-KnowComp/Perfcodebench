def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    PRIME = 1099511628211

    for row in rows:
        u = s = None
        f = None
        i = 0
        row_len = len(row)

        while i < row_len:
            # find next key start
            start = row.find('"', i)
            if start == -1:
                break
            end = row.find('"', start + 1)
            if end == -1:
                break
            key = row[start+1:end]

            # colon
            col = row.find(':', end + 1)
            if col == -1:
                break

            # skip whitespace after colon
            val_start = col + 1
            while val_start < row_len and row[val_start] in ' \t\n\r':
                val_start += 1

            if key == 'u':
                # parse non‑negative integer
                val_end = val_start
                while val_end < row_len and row[val_end].isdigit():
                    val_end += 1
                u = int(row[val_start:val_end])
                i = val_end
            elif key == 's':
                # parse possibly negative integer
                val_end = val_start
                if row[val_end] == '-':
                    val_end += 1
                while val_end < row_len and row[val_end].isdigit():
                    val_end += 1
                s = int(row[val_start:val_end])
                i = val_end
            elif key == 'f':
                # parse boolean
                if row[val_start:val_start+4] == 'true':
                    f = True
                    i = val_start + 4
                elif row[val_start:val_start+5] == 'false':
                    f = False
                    i = val_start + 5
                else:
                    # fallback – move past unknown value
                    i = row.find(',', val_start)
                    if i == -1:
                        i = row_len
                    else:
                        i += 1
            else:
                # unknown key – skip to next comma or end
                next_comma = row.find(',', val_start)
                if next_comma != -1:
                    i = next_comma + 1
                else:
                    break

            if u is not None and s is not None and f is not None:
                break

        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= PRIME
        h &= MASK

    return h