def run(deltas: list[int], iters: int) -> int:
    total = 0
    n = len(deltas)
    # Pre-calculate the constant to add to each element
    offset = 0x9E3779B1
    
    # The original logic calculates the sum of (prefix_sum[i] + offset) for all i
    # in every iteration. Since the deltas don't change, the sum is constant across iterations.
    # We can calculate the sum for one iteration and multiply by iters.
    
    current_prefix = 0
    iteration_sum = 0
    for d in deltas:
        current_prefix += d
        iteration_sum += (current_prefix + offset)
    
    return iteration_sum * iters