from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    # Pre-calculate the constant offset sum to avoid repeated addition
    n = len(deltas)
    offset_sum = n * 0x9E3779B1
    
    total = 0
    for _ in range(iters):
        # accumulate is implemented in C and is significantly faster than a Python loop
        # for prefix sum calculation.
        current_sum = sum(accumulate(deltas))
        total += current_sum + offset_sum
        
    return total