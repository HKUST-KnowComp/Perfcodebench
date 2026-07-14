def run(deltas: list[int], iters: int) -> int:
    len_deltas = len(deltas)
    constant = 0x9E3779B1
    total_offset = len_deltas * constant
    total = 0
    for _ in range(iters):
        current = 0
        sum_prefix = 0
        for d in deltas:
            current += d
            sum_prefix += current
        total = sum_prefix + total_offset
    return total