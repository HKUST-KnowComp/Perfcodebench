def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # Pre-calculate the constant to add to each element
    offset = 0x9E3779B1
    
    # We need to return the sum of (prefix_sum[i] + offset) for all i
    # Total = sum(prefix_sum[i]) + n * offset
    
    # To optimize, we observe that the prefix sum is calculated identically 
    # in every iteration. The 'iters' loop just repeats the same calculation.
    # However, the task implies we must simulate the process or the result 
    # is the sum of the final state. 
    # Looking at the baseline: total is reset to 0 inside the iters loop.
    # This means only the LAST iteration's 'total' is returned.
    
    total_prefix_sum = 0
    current = 0
    for d in deltas:
        current += d
        total_prefix_sum += current
    
    # The baseline logic: 
    # for _ in range(iters):
    #    ... calculate values ...
    #    total = 0
    #    for v in values: total += v + offset
    # return total
    
    # This means the 'iters' loop is actually redundant for the final result
    # because 'total' is overwritten every time. 
    # The result is simply the sum of (prefix_sum[i] + offset) from the last iteration.
    
    return total_prefix_sum + (n * offset)