def run(left: list[int], right: list[int], iters: int) -> int:
    # Pre-calculate the popcount of the intersection of the two bitsets
    # since the inputs 'left' and 'right' do not change across iterations.
    total_popcount = 0
    for a, b in zip(left, right):
        total_popcount += (a & b).bit_count()
    
    # The original code resets 'total' to 0 inside the loop and then sums
    # the bits_arr. This means the result of the function is simply the
    # popcount of the AND of the two lists, regardless of the number of iterations
    # (as long as iters > 0). If iters == 0, the original code would return 0.
    if iters <= 0:
        return 0
        
    return total_popcount