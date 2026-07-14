def run(left: list[int], right: list[int], iters: int) -> int:
    # The original code had a loop over 'iters', but it reset 'total' inside the loop.
    # This means only the final iteration actually contributes to the return value.
    # We can optimize by calculating the popcount of the intersection once.
    total = 0
    for a, b in zip(left, right):
        total += (a & b).bit_count()
    
    # Since the original code resets total = 0 at the start of each iteration,
    # the result is simply the popcount of the AND of the two lists.
    # If iters is 0, the original code would return 0.
    if iters == 0:
        return 0
        
    return total