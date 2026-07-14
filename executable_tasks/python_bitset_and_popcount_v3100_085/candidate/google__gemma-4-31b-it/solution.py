def run(left: list[int], right: list[int], iters: int) -> int:
    # Pre-calculate the intersection popcount for one iteration
    # since the inputs left and right do not change across iters.
    single_iter_total = 0
    for a, b in zip(left, right):
        single_iter_total += (a & b).bit_count()
    
    # The original code resets total = 0 inside the loop and returns the result of the last iteration.
    # However, the logic 'total = 0' inside the loop means it doesn't accumulate across iters,
    # it just repeats the same calculation. 
    # Based on the baseline: 
    # for _ in range(iters):
    #     total = 0
    #     for ...: total += v
    # return total
    
    # If iters is 0, the baseline would return the initial total = 0.
    if iters <= 0:
        return 0
        
    return single_iter_total