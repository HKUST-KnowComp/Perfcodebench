from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    n = len(deltas)
    prefix = list(accumulate(deltas))
    prefix_sum = sum(prefix)
    total = prefix_sum + n * 0x9E3779B1
    return total
