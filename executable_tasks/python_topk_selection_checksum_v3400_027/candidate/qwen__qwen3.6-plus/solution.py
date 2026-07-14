def run(scores: list[int], k: int, iters: int) -> int:
    MASK64 = 0xFFFFFFFFFFFFFFFF
    FNV_PRIME = 1099511628211
    FNV_OFFSET = 1469598103934665603
    
    # Create (-score, index) pairs to sort by score descending, then index ascending
    # Direct tuple comparison avoids the overhead of a Python key function
    pairs = [(-s, i) for i, s in enumerate(scores)]
    pairs.sort()
    
    # Precompute the exact XOR values needed for the checksum
    # This removes bit shifts, negations, and tuple unpacking from the hot loop
    xor_vals = [(-neg_s << 32) ^ i for neg_s, i in pairs[:k]]
    
    h = 0
    for _ in range(iters):
        h = FNV_OFFSET
        for x in xor_vals:
            h ^= x
            h = (h * FNV_PRIME) & MASK64
    return h