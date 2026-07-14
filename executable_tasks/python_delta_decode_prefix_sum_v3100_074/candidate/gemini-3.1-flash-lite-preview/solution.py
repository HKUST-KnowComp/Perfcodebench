def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    offset = 0x9E3779B1
    total_sum = 0
    
    for _ in range(iters):
        current = 0
        iteration_sum = 0
        # Fusing the prefix sum calculation and the aggregation
        # to avoid list allocation and multiple passes.
        for d in deltas:
            current += d
            iteration_sum += current
        
        total_sum += iteration_sum + (n * offset)
        
    return total_sum