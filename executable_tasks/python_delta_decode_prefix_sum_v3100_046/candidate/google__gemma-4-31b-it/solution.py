def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    # S = sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    prefix_sum_total = 0
    for i, d in enumerate(deltas):
        prefix_sum_total += d * (n - i)
    
    # Each iteration adds the prefix sum total and (n * 0x9E3779B1)
    constant_offset = n * 0x9E3779B1
    return iters * (prefix_sum_total + constant_offset)