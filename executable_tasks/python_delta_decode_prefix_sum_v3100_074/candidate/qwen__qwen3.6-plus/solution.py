from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    return sum(accumulate(deltas)) + len(deltas) * 0x9E3779B1
