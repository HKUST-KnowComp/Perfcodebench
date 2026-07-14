def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # Pre-calculate the constant offset sum to avoid repeated addition in the loop
    offset_sum = n * 0x9E3779B1
    total_sum = 0
    
    # Convert to local variable for faster access in the loop
    # Using a local reference to deltas can slightly speed up access
    d_list = deltas
    
    for _ in range(iters):
        current_prefix = 0
        iteration_total = 0
        for d in d_list:
            current_prefix += d
            iteration_total += current_prefix
        total_sum += iteration_total + offset_sum
        
    return total_sum