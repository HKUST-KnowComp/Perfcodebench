import itertools

def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    
    # The result is invariant across iterations, so we only need to compute it once.
    # We fuse the prefix sum generation and the total summation using itertools.accumulate.
    # The constant 0x9E3779B1 added to each element is factored out and multiplied by the length.
    return sum(itertools.accumulate(deltas)) + len(deltas) * 0x9E3779B1
