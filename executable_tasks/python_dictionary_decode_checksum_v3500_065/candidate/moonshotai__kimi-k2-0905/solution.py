import functools
import operator

def run(ids, dict_vals, iters: int) -> int:
    # Cache the raw value that each id will map to
    vals = [dict_vals[i] for i in ids]
    
    # Pre-compute the hash contribution of every element
    def elem_hash(x: int) -> int:
        h = 1469598103934665603
        h ^= x + 1
        h = (h * 1099511628211) & ((1 << 64) - 1)
        return h
    
    contrib = [elem_hash(v) for v in vals]
    
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        # XOR reduction over all pre-computed contributions
        h ^= functools.reduce(operator.xor, contrib, 0)
    return h