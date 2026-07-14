from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    magic = 0x9E3779B1
    offset = n * magic
    total = 0
    for _ in range(iters):
        prefix_sum = sum(accumulate(deltas))
        total = prefix_sum + offset
    return total
