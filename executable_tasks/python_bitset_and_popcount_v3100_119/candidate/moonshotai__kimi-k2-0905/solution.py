def run(left: list[int], right: list[int], iters: int) -> int:
    # Pre-compute the AND of every pair once -- it is loop-invariant.
    anded = [a & b for a, b in zip(left, right)]

    # One pass per iteration: just add the popcount of each word.
    total = 0
    for _ in range(iters):
        total = 0
        for w in anded:
            total += w.bit_count()
    return total