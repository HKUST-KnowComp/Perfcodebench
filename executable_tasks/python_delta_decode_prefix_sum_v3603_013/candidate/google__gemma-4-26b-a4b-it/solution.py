def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # Pre-calculate the contribution weights for each delta.
    # The i-th delta contributes to all prefix sums from index i to n-1.
    # Therefore, it is added (n - i) times to the total sum.
    weights = [n - i for i in range(n)]
    
    # Pre-calculate the constant offset sum.
    # Each element in the values list has 0x9E3779B1 added to it.
    offset_sum = n * 0x9E3779B1
    
    total_sum = 0
    # We use a local reference for speed
    for _ in range(iters):
        current_iter_sum = 0
        # The sum of prefix sums is equivalent to sum(deltas[i] * (n - i))
        for i in range(n):
            current_iter_sum += deltas[i] * weights[i]
        
        total_sum += current_iter_sum + offset_sum
        
    return total_sum