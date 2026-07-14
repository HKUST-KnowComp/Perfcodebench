import itertools

def run(deltas: list[int], iters: int) -> int:
    if iters == 0 or not deltas:
        return 0
    return sum(itertools.accumulate(deltas)) + len(deltas) * 0x9E3779B1
