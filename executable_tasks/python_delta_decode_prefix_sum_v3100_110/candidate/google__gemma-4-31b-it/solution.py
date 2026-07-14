def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    # = sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    weighted_sum = 0
    for i, d in enumerate(deltas):
        weighted_sum += d * (n - i)
    
    # Each iteration adds the weighted_sum and (n * 0x9E3779B1)
    offset_per_iter = n * 0x9E3779B1
    total_per_iter = weighted_sum + offset_per_iter
    
    return total_per_iter * iters