def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    offset = 0x9E3779B1
    total_sum = 0
    
    for _ in range(iters):
        current = 0
        iteration_sum = 0
        # Fusing prefix sum calculation and aggregation
        # Sum of prefix sums: sum_{i=1}^n (sum_{j=1}^i d_j)
        # This is equivalent to sum_{i=1}^n ((n - i + 1) * d_i)
        for d in deltas:
            current += d
            iteration_sum += current
        
        total_sum += iteration_sum + (n * offset)
        
    return total_sum