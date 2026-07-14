def run(ids, dict_vals, iters: int) -> int:
    # Pre-decode once and pre-add 1 to avoid repeated work inside the hot loop
    dv = dict_vals
    wvals = [dv[idx] + 1 for idx in ids]

    # FNV-1a-like parameters
    offset = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    h = 0  # Must return 0 when iters == 0 (matches baseline behavior)
    for _ in range(iters):
        hv = offset
        for w in wvals:
            hv ^= w
            hv = (hv * prime) & mask
        h = hv
    return h
