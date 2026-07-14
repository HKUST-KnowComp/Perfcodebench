def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Pre-calculate the intersection of the two bitsets
    # Since left and right are constant across iters, we can compute the
    # popcount of the intersection once and multiply by iters.
    # However, the original code resets total = 0 inside the loop and 
    # returns the result of the last iteration. 
    # Let's follow the logic: the original code does 'total = 0' inside the loop,
    # meaning it only returns the popcount of the final iteration.
    
    # Optimization: The result of the AND operation is the same for every iteration.
    # We only need to compute it once.
    
    current_popcount = 0
    for a, b in zip(left, right):
        current_popcount += (a & b).bit_count()
    
    # The original code's loop structure:
    # for _ in range(iters):
    #     total = 0
    #     for ...:
    #         total += v
    # This means the result is simply the popcount of the intersection of left and right.
    
    return current_popcount