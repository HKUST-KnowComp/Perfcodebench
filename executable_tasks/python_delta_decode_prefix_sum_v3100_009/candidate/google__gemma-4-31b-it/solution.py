def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    # S = sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    weighted_sum = 0
    for i, d in enumerate(deltas):
        weighted_sum += d * (n - i)
    
    # Each iteration adds the weighted_sum and n * 0x9E3779B1
    # Total = iters * (weighted_sum + n * 0x9E3779B1)
    return iters * (weighted_sum + n * 0x9E3779B1)