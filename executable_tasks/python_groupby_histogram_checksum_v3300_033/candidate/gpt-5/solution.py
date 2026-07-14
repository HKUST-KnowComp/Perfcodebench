def run(keys, vals, buckets: int, iters: int) -> int:
    # If no iterations, match baseline behavior (returns 0)
    if iters <= 0:
        return 0

    # Build the histogram once (result is the same for every iteration in baseline)
    hist = [0] * buckets

    klist = keys
    vlist = vals
    hk = hist

    n = len(klist)
    i = 0
    limit = n - (n % 4)

    # Unrolled accumulation for performance
    while i < limit:
        k0 = klist[i]
        hk[k0] += vlist[i]
        k1 = klist[i + 1]
        hk[k1] += vlist[i + 1]
        k2 = klist[i + 2]
        hk[k2] += vlist[i + 2]
        k3 = klist[i + 3]
        hk[k3] += vlist[i + 3]
        i += 4

    # Remainder
    while i < n:
        hk[klist[i]] += vlist[i]
        i += 1

    # Hash the histogram once; baseline resets seed each iter and hashes the same data
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211

    for v in hk:
        h ^= v + 1
        h = (h * mul) & mask

    return h
