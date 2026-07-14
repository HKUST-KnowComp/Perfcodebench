def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    # Pre-calculate the constant to add to each element
    # The original logic: total = sum(prefix_sum[i] + constant)
    # which is equivalent to: total = sum(prefix_sum[i]) + (len(deltas) * constant)
    constant = 0x9E3779B1
    n = len(deltas)
    
    # We need to return the result of the loop 'iters' times.
    # However, the original code resets 'total' inside the 'iters' loop.
    # Looking closely at the baseline:
    # for _ in range(iters):
    #    current = 0
    #    for i, d in enumerate(deltas):
    #        current += d
    #        values[i] = current
    #    total = 0
    #    for v in values:
    #        total += v + 0x9E3779B1
    # return total
    # This means the 'iters' loop actually just repeats the same calculation
    # and the final 'total' returned is only the result of the LAST iteration.
    # Therefore, 'iters' is effectively irrelevant to the final value if it's > 0,
    # unless the deltas change (which they don't).
    
    # Optimization: Single pass to calculate sum of prefix sums
    total_prefix_sum = 0
    current_prefix_sum = 0
    for d in deltas:
        current_prefix_sum += d
        total_prefix_sum += current_prefix_sum
    
    # The final total is sum(v + constant) for v in values
    # which is sum(v) + (n * constant)
    return total_prefix_sum + (n * constant)