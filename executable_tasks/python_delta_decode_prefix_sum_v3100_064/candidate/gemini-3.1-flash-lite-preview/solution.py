def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    offset = 0x9E3779B1
    total_sum = 0
    
    for _ in range(iters):
        current_val = 0
        iteration_sum = 0
        for d in deltas:
            current_val += d
            iteration_sum += current_val
        
        total_sum += iteration_sum + (n * offset)
        
    return total_sum