def run(rows: list[str]) -> int:
    # 64-bit FNV-1a parameters
    h = 1469598103934665603
    FNV_PRIME = 1099511628211

    for row in rows:
        n = len(row)
        i = 0
        # Values for keys; first occurrence is used
        u = None  # type: ignore
        s = None  # type: ignore
        f = None  # type: ignore
        found = 0  # bitmask: 1=u, 2=score, 4=flag

        while i < n:
            # Parse key from i up to '=' or separator
            j = i
            while j < n:
                ch = row[j]
                if ch == '=' or ch == '&' or ch == ';':
                    break
                j += 1

            key_len = j - i
            # Determine key code
            key_code = 0
            if key_len == 1:
                # key == 'u'
                if row[i] == 'u':
                    key_code = 1
            elif key_len == 5:
                # key == 'score'
                # Compare char-by-char to avoid slicing
                if (row[i] == 's' and row[i + 1] == 'c' and row[i + 2] == 'o' and
                        row[i + 3] == 'r' and row[i + 4] == 'e'):
                    key_code = 2
            elif key_len == 4:
                # key == 'flag'
                if (row[i] == 'f' and row[i + 1] == 'l' and
                        row[i + 2] == 'a' and row[i + 3] == 'g'):
                    key_code = 4

            # If no '=', skip this token
            if j >= n or row[j] != '=':
                # Move to next token after separator if any
                if j < n and (row[j] == '&' or row[j] == ';'):
                    i = j + 1
                else:
                    i = n
                continue

            # Parse value starting at j+1 until next separator (& or ;) or end
            k = j + 1
            if k >= n:
                # empty value; skip
                i = k
                continue

            neg = False
            ch0 = row[k]
            if ch0 == '-' or ch0 == '+':
                neg = (ch0 == '-')
                k += 1

            # Accumulate integer digits
            val = 0
            start_digits = k
            has_digit = False
            while k < n:
                ch = row[k]
                # Fast digit check without ord for common ASCII
                if '0' <= ch <= '9':
                    has_digit = True
                    val = val * 10 + (ord(ch) - 48)
                    k += 1
                else:
                    break

            if has_digit:
                if neg:
                    val = -val
                # Store only first occurrence per key
                if key_code == 1:
                    if (found & 1) == 0:
                        u = val
                        found |= 1
                elif key_code == 2:
                    if (found & 2) == 0:
                        s = val
                        found |= 2
                elif key_code == 4:
                    if (found & 4) == 0:
                        f = val
                        found |= 4

            # Skip non-digit remainder of value until next separator
            while k < n and row[k] not in '&;':
                k += 1
            i = k + 1 if k < n else n

            if found == 7:
                break

        # Compute checksum piece
        # Assumes required fields are present as per problem spec
        v = (u & 2047) + s + f * 19  # type: ignore
        h ^= (v + 1)
        h = (h * FNV_PRIME) & 0xFFFFFFFFFFFFFFFF

    return h
