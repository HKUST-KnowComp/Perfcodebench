def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        i = 0
        n = len(row)
        # skip to first '{'
        while i < n and row[i] != '{':
            i += 1
        i += 1  # after '{'
        u = None
        s = None
        f = False
        while i < n:
            # skip whitespace
            while i < n and row[i] in ' \t\n\r':
                i += 1
            if i >= n or row[i] == '}':
                break
            # expect '"'
            if row[i] != '"':
                i += 1
                continue
            key_start = i + 1
            i = row.index('"', key_start)
            key = row[key_start:i]
            i += 1  # after closing quote
            # skip to colon
            while i < n and row[i] != ':':
                i += 1
            i += 1  # after ':'
            # skip whitespace after colon
            while i < n and row[i] in ' \t\n\r':
                i += 1
            if key == 'u':
                val_start = i
                while i < n and row[i] not in ',} \t\n\r':
                    i += 1
                u = int(row[val_start:i])
            elif key == 's':
                val_start = i
                while i < n and row[i] not in ',} \t\n\r':
                    i += 1
                s = int(row[val_start:i])
            elif key == 'f':
                if row[i:i+4] == 'true':
                    f = True
                    i += 4
                elif row[i:i+5] == 'false':
                    f = False
                    i += 5
            else:
                # skip unknown value
                while i < n and row[i] not in ',}':
                    i += 1
            # skip comma and whitespace
            while i < n and row[i] in ', \t\n\r':
                i += 1
        # Compute v using the extracted fields
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h