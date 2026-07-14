def run(left: list[int], right: list[int], iters: int) -> int:
    # Pre-calculate the intersection popcount for one iteration
    # since the inputs left and right do not change across iters.
    single_iter_total = 0
    for a, b in zip(left, right):
        single_iter_total += (a & b).bit_count()
    
    # The original code resets total = 0 inside the iters loop,
    # meaning it only returns the result of the final iteration.
    # However, the logic 'total = 0' inside the loop suggests it's
    # calculating the popcount of the AND of the two lists.
    # Based on the baseline: total is reset every iteration.
    return single_iter_total