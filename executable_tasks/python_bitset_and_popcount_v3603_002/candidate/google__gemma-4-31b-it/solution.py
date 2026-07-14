def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Pre-calculate the intersection popcount for one iteration
    # since the inputs left and right do not change across iters.
    single_pass_count = 0
    for a, b in zip(left, right):
        single_pass_count += (a & b).bit_count()
    
    # The original code resets total = 0 inside the loop and then
    # calculates the sum of the bits_arr. This means the final result
    # is simply the popcount of the AND of the two lists for the last iteration.
    # However, the original code's logic is: 
    # for _ in range(iters): 
    #     ... calculate bits_arr ...
    #     total = 0
    #     for v in bits_arr: total += v
    # This means the result is just the popcount of the intersection of left and right.
    
    return single_pass_count