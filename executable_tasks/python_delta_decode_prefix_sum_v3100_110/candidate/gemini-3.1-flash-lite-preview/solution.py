import itertools

def run(deltas: list[int], iters: int) -> int:
    # Pre-calculate the constant offset sum to avoid repeated addition
    # sum(v + C) = sum(v) + len(deltas) * C
    n = len(deltas)
    offset_sum = n * 0x9E3779B1
    
    total = 0
    for _ in range(iters):
        # itertools.accumulate is implemented in C and is significantly faster 
        # than a manual Python loop for prefix sums.
        prefix_sum = sum(itertools.accumulate(deltas))
        total += (prefix_sum + offset_sum)
        
    return total