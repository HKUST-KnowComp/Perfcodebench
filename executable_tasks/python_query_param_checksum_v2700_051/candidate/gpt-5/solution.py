def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    for row in rows:
        n = len(row)
        i = 0
        u = None
        s = None
        f = None

        # Parse query string manually: key=value pairs separated by '&'
        while i < n:
            # Find end of key or delimiter
            k_start = i
            while i < n:
                c = row[i]
                if c == '=' or c == '&':
                    break
                i += 1
            k_end = i

            has_value = (i < n and row[i] == '=')
            if has_value:
                i += 1  # skip '='
                v_start = i
                # Find end of value
                while i < n and row[i] != '&':
                    i += 1
                v_end = i

                # Match keys and parse integer values (first occurrence wins)
                klen = k_end - k_start
                # Fast key matching without creating substrings when possible
                if u is None and klen == 1 and row[k_start] == 'u':
                    # parse integer from row[v_start:v_end]
                    j = v_start
                    neg = False
                    if j < v_end:
                        ch = row[j]
                        if ch == '-':
                            neg = True
                            j += 1
                        elif ch == '+':
                            j += 1
                    val = 0
                    while j < v_end:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    if neg:
                        val = -val
                    u = val
                elif s is None and klen == 5 and row[k_start:k_end] == 'score':
                    j = v_start
                    neg = False
                    if j < v_end:
                        ch = row[j]
                        if ch == '-':
                            neg = True
                            j += 1
                        elif ch == '+':
                            j += 1
                    val = 0
                    while j < v_end:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    if neg:
                        val = -val
                    s = val
                elif f is None and klen == 4 and row[k_start:k_end] == 'flag':
                    j = v_start
                    neg = False
                    if j < v_end:
                        ch = row[j]
                        if ch == '-':
                            neg = True
                            j += 1
                        elif ch == '+':
                            j += 1
                    val = 0
                    while j < v_end:
                        val = val * 10 + (ord(row[j]) - 48)
                        j += 1
                    if neg:
                        val = -val
                    f = val
            # Skip '&' if present and continue
            if i < n and row[i] == '&':
                i += 1

            if u is not None and s is not None and f is not None:
                break

        # Compute checksum contribution
        # Assumes required keys are present in data
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * prime) & mask

    return h
