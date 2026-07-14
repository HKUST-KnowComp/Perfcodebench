def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = d0 + (d0 + d1) + (d0 + d1 + d2) ...
    # S = sum_{i=0}^{n-1} (n - i) * deltas[i]
    
    prefix_sum_total = 0
    for i, d in enumerate(deltas):
        prefix_sum_total += d * (n - i)
    
    # Each iteration adds the prefix sum total and n * 0x9E3779B1
    constant_offset = n * 0x9E3779B1
    return iters * (prefix_sum_total + constant_offset)