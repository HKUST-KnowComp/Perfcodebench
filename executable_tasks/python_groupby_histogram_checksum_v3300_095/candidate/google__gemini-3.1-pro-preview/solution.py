def run(keys, vals, buckets: int, iters: int) -> int:
    # The baseline implementation recomputes the histogram and checksum 'iters' times,
    # but since 'h' is reset to the FNV offset basis inside the loop, only the last 
    # iteration's result is returned. We can optimize by computing it once.
    
    # Pre-allocate the dense histogram
    hist = [0] * buckets
    
    # Aggregate keyed values into the histogram
    # zip() is generally the most efficient way to iterate over two lists in pure Python
    for k, v in zip(keys, vals):
        hist[k] += v

    # FNV-1a constants
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF

    # Compute the checksum over the buckets
    h = FNV_OFFSET_BASIS
    for v in hist:
        h ^= (v + 1)
        h = (h * FNV_PRIME) & MASK
        
    return h