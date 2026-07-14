import itertools

def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    return len(deltas) * 0x9E3779B1 + sum(itertools.accumulate(deltas))
