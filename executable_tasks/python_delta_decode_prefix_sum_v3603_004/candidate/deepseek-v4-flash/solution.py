def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    C = 0x9E3779B1
    # base_sum = n * C + sum_{i=0}^{n-1} prefix[i], where prefix[i] = sum_{j=0}^{i} delta[j]
    # = n * C + sum_{j=0}^{n-1} delta[j] * (n - j)
    base_sum = n * C
    cumulative = 0
    for i, d in enumerate(deltas):
        cumulative += d
        base_sum += cumulative
    return base_sum * iters