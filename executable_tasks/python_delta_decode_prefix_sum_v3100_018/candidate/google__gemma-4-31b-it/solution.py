def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # sum_{i=0}^{n-1} (sum_{j=0}^{i} delta_j)
    # = sum_{j=0}^{n-1} (n - j) * delta_j
    
    weighted_sum = 0
    for i, d in enumerate(deltas):
        weighted_sum += (n - i) * d
    
    # Each iteration also adds (n * 0x9E3779B1) to the total
    constant_part = n * 0x9E3779B1
    
    # Total = iters * (weighted_sum + constant_part)
    return iters * (weighted_sum + constant_part)