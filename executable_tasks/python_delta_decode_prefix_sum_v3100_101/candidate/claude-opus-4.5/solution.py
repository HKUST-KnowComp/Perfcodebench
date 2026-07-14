from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    magic = 0x9E3779B1
    offset = n * magic
    total = 0
    for _ in range(iters):
        total = sum(accumulate(deltas)) + offset
    return total
