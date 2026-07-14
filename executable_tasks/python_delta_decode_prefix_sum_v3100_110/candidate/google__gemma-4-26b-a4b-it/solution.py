def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    n = len(deltas)
    total_sum = 0
    offset = 0x9E3779B1
    
    # Pre-calculate the sum of the constant offset to avoid repeated addition
    offset_contribution = n * offset
    
    # We need to track the prefix sums. 
    # The sum of prefix sums of [d0, d1, d2...] is:
    # P0 = d0
    # P1 = d0 + d1
    # P2 = d0 + d1 + d2
    # Sum = n*d0 + (n-1)*d1 + (n-2)*d2 + ... + 1*dn-1
    
    # Pre-calculate the weights for each delta to transform the problem
    # into a single dot product per iteration.
    weights = [0] * n
    current_weight = n
    for i in range(n):
        weights[i] = current_weight
        current_weight -= 1
        
    # The total sum for one iteration is sum(deltas[i] * weights[i]) + offset_contribution
    # However, the task implies the 'values' array is updated and used in the next iteration.
    # Wait, looking at the baseline: 'values[i] = current' updates the array.
    # In the next iteration of 'iters', 'current' starts at 0, but 'values' is overwritten.
    # This means the 'values' array does NOT carry over between 'iters' loops.
    # Each iteration is independent.
    
    # Let's re-verify the baseline logic:
    # for _ in range(iters):
    #    current = 0
    #    for i, d in enumerate(deltas):
    #        current += d
    #        values[i] = current
    #    total = 0
    #    for v in values:
    #        total += v + 0x9E3779B1
    # return total
    
    # The 'total' is overwritten every iteration. Only the LAST iteration's total is returned.
    # This is a crucial observation. The 'iters' loop is actually redundant if it just overwrites 'total'.
    # But if 'iters' is 0, it returns 0. If it's >= 1, it returns the result of the last iteration.
    
    if iters <= 0:
        return 0

    # Calculate the sum for a single iteration
    # Sum = sum_{i=0}^{n-1} (prefix_sum[i] + offset)
    # Sum = sum_{i=0}^{n-1} (sum_{j=0}^{i} deltas[j] + offset)
    # Sum = sum_{j=0}^{n-1} (deltas[j] * (n - j)) + (n * offset)
    
    single_iter_sum = offset_contribution
    for i in range(n):
        single_iter_sum += deltas[i] * weights[i]
        
    return single_iter_sum