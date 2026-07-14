def run(deltas: list[int], iters: int) -> int:
    if iters == 0:
        return 0
    total_prefix = 0
    cur = 0
    for d in deltas:
        cur += d
        total_prefix += cur
    total = total_prefix + len(deltas) * 0x9E3779B1
    return total