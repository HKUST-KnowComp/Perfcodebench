def run(rows: list[str]) -> int:
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    ws1 = ' \t\r\n'

    for s in rows:
        n = len(s)
        i = 0
        got_u = False
        got_s = False
        got_f = False
        u_val = 0
        s_val = 0
        f_truth = False

        # Single pass scan for keys "u", "s", "f"
        while i < n and not (got_u and got_s and got_f):
            i = s.find('"', i)
            if i == -1:
                break
            # Check pattern "k" where k in {u, s, f}
            if i + 2 < n:
                k = s[i + 1]
                if (k == 'u' or k == 's' or k == 'f') and s[i + 2] == '"':
                    j = i + 3
                    # skip whitespace
                    while j < n and s[j] in ws1:
                        j += 1
                    if j >= n or s[j] != ':':
                        i += 1
                        continue
                    j += 1
                    # skip whitespace before value
                    while j < n and s[j] in ws1:
                        j += 1
                    if j >= n:
                        break

                    cj = s[j]
                    if k == 'f':
                        # parse boolean or numeric truthiness
                        if cj == 't':
                            # expect true
                            # We assume valid JSON; minimal validation for speed
                            f_truth = True
                            got_f = True
                            # advance roughly past 'true'
                            j += 4
                        elif cj == 'f':
                            # expect false
                            f_truth = False
                            got_f = True
                            j += 5
                        else:
                            # parse numeric then take truthiness
                            neg = False
                            if cj == '-':
                                neg = True
                                j += 1
                            val = 0
                            while j < n:
                                ch = s[j]
                                d = ord(ch) - 48
                                if 0 <= d <= 9:
                                    val = val * 10 + d
                                    j += 1
                                else:
                                    break
                            if neg:
                                val = -val
                            f_truth = (val != 0)
                            got_f = True
                        i = j
                        continue
                    else:
                        # parse integer for u or s
                        neg = False
                        if cj == '-':
                            neg = True
                            j += 1
                        val = 0
                        while j < n:
                            ch = s[j]
                            d = ord(ch) - 48
                            if 0 <= d <= 9:
                                val = val * 10 + d
                                j += 1
                            else:
                                break
                        if neg:
                            val = -val
                        if k == 'u':
                            u_val = val
                            got_u = True
                        else:
                            s_val = val
                            got_s = True
                        i = j
                        continue
            i += 1

        v = (u_val & 1023) + s_val + (17 if f_truth else 0)
        h ^= v + 1
        h = (h * prime) & mask

    return h
