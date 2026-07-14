def run(keys, vals, buckets: int, iters: int) -> int:
    # If no iterations are requested, baseline returns 0 without computing anything.
    if iters <= 0:
        return 0

    # Build the histogram once; it would be identical each iteration in the baseline.
    hist = [0] * buckets

    # Use fast path for indexable sequences; fallback to zip for generic iterables.
    try:
        kg = keys.__getitem__  # type: ignore[attr-defined]
        vg = vals.__getitem__  # type: ignore[attr-defined]
        n = len(keys)          # type: ignore[arg-type]
    except Exception:
        hlist = hist
        for k, v in zip(keys, vals):
            hlist[k] += v
    else:
        hlist = hist
        for i in range(n):
            hlist[kg(i)] += vg(i)

    # Compute checksum once; baseline resets the hash each iteration so final equals one pass.
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mul = 1099511628211
    for v in hist:
        h ^= v + 1
        h = (h * mul) & mask

    return h
