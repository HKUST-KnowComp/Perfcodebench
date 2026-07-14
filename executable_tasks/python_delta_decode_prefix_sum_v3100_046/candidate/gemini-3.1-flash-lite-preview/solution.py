from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    # Pre-calculate the constant offset sum to avoid repeated addition in the loop
    # sum(v + C) = sum(v) + len(deltas) * C
    n = len(deltas)
    offset_sum = n * 0x9E3779B1
    
    total = 0
    for _ in range(iters):
        # accumulate is implemented in C and is significantly faster than a manual loop
        # for prefix sum calculation.
        prefix_sum = sum(accumulate(deltas))
        total += (prefix_sum + offset_sum)
        
    return total