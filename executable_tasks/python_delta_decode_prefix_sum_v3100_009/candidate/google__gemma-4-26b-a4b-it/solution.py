def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The goal is to compute sum_{i=0}^{n-1} (prefix_sum[i] + offset)
    # prefix_sum[i] = sum_{j=0}^{i} deltas[j]
    # Total sum = sum_{i=0}^{n-1} sum_{j=0}^{i} deltas[j] + n * offset
    # By changing the order of summation:
    # sum_{j=0}^{n-1} deltas[j] * (n - j)
    
    offset = 0x9E3779B1
    offset_total = n * offset
    
    # Pre-calculate the contribution of deltas to the prefix sum total
    # This part is constant across all 'iters'
    delta_contribution = 0
    for i, d in enumerate(deltas):
        delta_contribution += d * (n - i)
    
    # The loop in the original code repeats the same calculation 'iters' times.
    # However, the original code resets 'total = 0' inside the 'iters' loop,
    # but it actually accumulates the sum of the 'values' array calculated in the last iteration.
    # Wait, looking closely at the baseline:
    # for _ in range(iters):
    #    ... calculate values ...
    #    total = 0
    #    for v in values: total += v + offset
    # return total
    # This means the 'iters' loop is actually redundant if the deltas don't change,
    # because 'total' is overwritten every time. 
    # The final 'total' returned is just the result of the LAST iteration.
    
    # Let's re-verify the baseline logic:
    # for _ in range(iters):
    #    ... (updates values) ...
    #    total = 0
    #    for v in values: total += v + offset
    # return total
    # Yes, 'total' is reset to 0 inside the loop. The 'iters' loop effectively
    # just performs the same calculation 'iters' times and returns the last one.
    
    return delta_contribution + offset_total