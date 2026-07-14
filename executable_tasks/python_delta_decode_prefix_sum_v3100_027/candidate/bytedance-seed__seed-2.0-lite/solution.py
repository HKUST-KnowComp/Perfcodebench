def run(deltas: list[int], iters: int) -> int:
    current_prefix = 0
    sum_of_prefixes = 0
    for d in deltas:
        current_prefix += d
        sum_of_prefixes += current_prefix
    per_iter_constant = 0x9E3779B1 * len(deltas)
    return (sum_of_prefixes + per_iter_constant) * iters