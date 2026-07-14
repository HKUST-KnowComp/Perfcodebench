def run(ids, dict_vals, iters: int) -> int:
    h = 0
    # Pre-cache dict_vals lookup to local variable for faster access
    # Using a local reference to the dict_vals list
    vals = dict_vals
    
    for _ in range(iters):
        h = 1469598103934665603
        # Inline the hash calculation to avoid list creation and extra loops
        for idx in ids:
            h ^= (vals[idx] + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h