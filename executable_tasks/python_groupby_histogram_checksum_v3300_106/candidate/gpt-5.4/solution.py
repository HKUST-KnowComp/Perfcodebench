def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    hist = [0] * buckets
    kseq = keys
    vseq = vals

    for _ in range(iters):
        # Reset in place to avoid reallocating the histogram list each iteration.
        hist[:] = [0] * buckets

        # Index-based loop is typically faster than zip() iteration for this workload.
        for i in range(len(kseq)):
            hist[kseq[i]] += vseq[i]

        h = offset
        for v in hist:
            h = ((h ^ (v + 1)) * prime) & mask

    return h
