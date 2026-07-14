from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    total = sum(accumulate(deltas)) + n * 0x9E3779B1
    return total * iters
