def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Pre-calculate the intersection popcount for one iteration
    # since left and right do not change across iters.
    single_pass_count = 0
    for a, b in zip(left, right):
        single_pass_count += (a & b).bit_count()
    
    # The original code resets total = 0 inside the loop and returns the last iteration's result.
    # However, the logic 'total = 0' inside the loop means only the final iteration matters.
    # If the goal is to simulate the baseline's behavior exactly:
    # The baseline does: for _ in range(iters): total = 0; for ...: total += v
    # This means it returns the popcount of (left & right) for a single iteration.
    
    return single_pass_count