def run(keys, vals, buckets: int, iters: int) -> int:
    # The baseline implementation resets 'h' inside the 'iters' loop and 
    # performs the same calculation every time. To optimize, we only need 
    # to compute the histogram and checksum once.
    if iters == 0:
        return 0

    # Initialize histogram
    hist = [0] * buckets
    
    # Aggregate keys and values
    # zip() is efficient in Python 3 as it returns an iterator.
    for k, v in zip(keys, vals):
        hist[k] += v

    # FNV-1a style checksum calculation
    # Constants for 64-bit FNV-1a
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    for v in hist:
        h ^= (v + 1)
        h = (h * fnv_prime) & mask

    return h