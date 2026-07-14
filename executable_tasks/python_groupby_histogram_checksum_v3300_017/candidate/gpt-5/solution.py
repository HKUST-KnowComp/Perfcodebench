def run(keys, vals, buckets: int, iters: int) -> int:
    # Constants for 64-bit FNV-1a-like checksum
    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1

    # Reuse accumulation storage across iterations using a stamp array
    # sums holds bucket sums; stamps marks which iteration last wrote a bucket
    sums = [0] * buckets
    stamps = [0] * buckets
    stamp = 1  # current iteration stamp (start at 1, since stamps initialized to 0)

    keys_l = keys
    vals_l = vals
    n = len(keys_l)

    h = 0
    for _ in range(iters):
        sl = sums
        st = stamps
        s = stamp

        # Accumulate values into buckets with stamp check
        i = 0
        # cache length locally for speed
        ln = n
        while i < ln:
            k = keys_l[i]
            v = vals_l[i]
            if st[k] == s:
                sl[k] += v
            else:
                st[k] = s
                sl[k] = v
            i += 1

        # Compute checksum over all buckets treating untouched as 0
        hval = fnv_offset
        j = 0
        b = buckets
        prime = fnv_prime
        m = mask
        while j < b:
            if st[j] == s:
                vj = sl[j]
            else:
                vj = 0
            hval = ((hval ^ (vj + 1)) * prime) & m
            j += 1
        h = hval

        stamp = s + 1

    return h
