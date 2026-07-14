def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    offset = 0x9E3779B1
    total_sum = 0
    
    # Pre-calculate the constant contribution to the total sum
    # Each of the n elements is added with offset once per iteration
    constant_contribution = n * offset
    
    for _ in range(iters):
        current = 0
        iteration_sum = 0
        for d in deltas:
            current += d
            iteration_sum += current
        total_sum += (iteration_sum + constant_contribution)
        
    return total_sum