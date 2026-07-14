def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    # Pre-calculate the constant to add to each decoded value
    offset = 0x9E3779B1
    total_sum = 0
    
    # The core bottleneck is the nested loop. 
    # We can optimize by realizing that for each iteration, 
    # we are calculating the sum of prefix sums.
    # However, the task requires the exact sum of (prefix_sum[i] + offset).
    
    # To maintain the exact logic of the baseline:
    # For each iteration:
    #   current = 0
    #   for d in deltas: current += d; values[i] = current
    #   total = sum(v + offset for v in values)
    
    # Optimization: The 'values' array is overwritten every iteration.
    # We can compute the sum of the current 'values' array directly.
    
    # Pre-calculate length to avoid repeated len() calls
    n = len(deltas)
    
    for _ in range(iters):
        current = 0
        iteration_sum = 0
        for d in deltas:
            current += d
            iteration_sum += current
        
        # The baseline does: total = sum(v + offset for v in values)
        # which is equivalent to: sum(v) + (n * offset)
        total_sum += iteration_sum + (n * offset)
        
    return total_sum