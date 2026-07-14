def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    C = 0x9E3779B1
    total_prefix_sum = 0
    weight = n
    for d in deltas:
        total_prefix_sum += d * weight
        weight -= 1
    return iters * (total_prefix_sum + n * C)
