def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    
    # Sort by score descending, then index ascending using native tuple comparison
    neg_pairs = [(-s, i) for i, s in enumerate(scores)]
    neg_pairs.sort()
    
    # Precompute hash input values to avoid repeated shifts/xors inside the loop
    vals = tuple(((-ns) << 32) ^ i for ns, i in neg_pairs[:k])
    
    h = 0
    PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    INIT = 1469598103934665603
    
    for _ in range(iters):
        h = INIT
        for v in vals:
            h = ((h ^ v) * PRIME) & MASK
            
    return h