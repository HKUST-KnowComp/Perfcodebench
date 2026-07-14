def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    # compute prefix sums once
    prefix = [0] * n
    cur = 0
    for i, d in enumerate(deltas):
        cur += d
        prefix[i] = cur
    total = sum(prefix) + n * 0x9E3779B1
    return total