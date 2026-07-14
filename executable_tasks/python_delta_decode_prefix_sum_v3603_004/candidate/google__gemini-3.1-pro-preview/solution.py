from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    return sum(accumulate(deltas)) + len(deltas) * 0x9E3779B1
