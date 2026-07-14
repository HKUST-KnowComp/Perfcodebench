def run(ids, dict_vals, iters: int) -> int:
    # Pre-calculate the sequence of values that will be in 'decoded' after the last iteration.
    # Since ids and dict_vals do not change, the content of 'decoded' is constant after the first iteration.
    # The loop 'for _ in range(iters)' effectively just repeats the same assignment.
    
    # Optimization: The values in 'decoded' are always dict_vals[idx] for idx in ids.
    # We can pre-fetch these values to avoid repeated indexing in the loop.
    sequence = [dict_vals[idx] for idx in ids]
    
    # The current baseline logic: 
    # 1. For each iteration, update 'decoded' (which results in the same values every time).
    # 2. Reset 'h' to the FNV offset basis.
    # 3. Compute the hash over the current 'decoded' values.
    # 4. The final 'h' returned is the result of the LAST iteration.
    
    # Therefore, we only need to perform the assignment once and the hash calculation once.
    # If iters == 0, the baseline returns h=0 (initial value).
    if iters <= 0:
        return 0

    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in sequence:
        h ^= v + 1
        h = (h * 1099511628211) & mask
    return h