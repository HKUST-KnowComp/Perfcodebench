def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # Pre-calculate the contribution of each delta to the total prefix sum.
    # The i-th delta (0-indexed) is added to all prefix sums from index i to n-1.
    # Therefore, it is added (n - i) times to the total sum of prefix sums.
    # We also need to account for the constant 0x9E3779B1 added to each prefix sum.
    
    # Pre-calculate the weighted sum of deltas
    weighted_delta_sum = 0
    for i in range(n):
        weighted_delta_sum += deltas[i] * (n - i)
    
    # The constant part: each of the n elements has 0x9E3779B1 added to it.
    constant_offset = n * 0x9E3779B1
    
    # Total sum for one iteration
    iteration_total = weighted_delta_sum + constant_offset
    
    # Since the deltas and the process are deterministic and don't change between iters,
    # we can simply multiply the single iteration result by iters.
    return iteration_total * iters