def run(left: list[int], right: list[int], iters: int) -> int:
    # Pre-calculate the intersection popcount for one iteration
    # since the inputs left and right do not change across iters.
    single_iter_total = 0
    for a, b in zip(left, right):
        single_iter_total += (a & b).bit_count()
    
    # The original code resets total = 0 inside the iters loop,
    # meaning it only returns the result of the final iteration.
    # However, the logic 'total = 0' inside the loop suggests the 
    # intended result is the popcount of the final pass.
    # If the goal was cumulative, it would be single_iter_total * iters.
    # Based on the baseline: total is reset to 0 at the start of the 
    # final popcount loop, so it returns the result of one iteration.
    return single_iter_total