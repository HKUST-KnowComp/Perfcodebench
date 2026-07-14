def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    n = len(deltas)
    total_sum = 0
    
    # Pre-calculate the constant addition part
    # The original code adds 0x9E3779B1 to every element in the decoded list.
    # Total = sum(prefix_sums) + (n * 0x9E3779B1)
    constant_offset = n * 0x9E3779B1
    
    for _ in range(iters):
        current_prefix = 0
        current_total = 0
        for d in deltas:
            current_prefix += d
            current_total += current_prefix
        total_sum = current_total + constant_offset
        
    return total_sum