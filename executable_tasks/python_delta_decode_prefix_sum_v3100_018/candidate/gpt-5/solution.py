def run(deltas: list[int], iters: int) -> int:
    # Preserve baseline behavior: if no iterations, return 0.
    if iters <= 0:
        return 0

    n = len(deltas)
    # Constant added to each decoded value
    const = 0x9E3779B1

    # The total for one iteration equals:
    #   sum_{i=0..n-1} (prefix_i + const)
    # where prefix_i = sum_{j=0..i} deltas[j]
    # Sum of prefixes equals sum_{j=0..n-1} (n - j) * deltas[j]
    # Therefore, total = n*const + sum((n - i) * deltas[i])

    res = n * const
    m = n
    # Efficient single pass without building intermediate values
    for d in deltas:
        res += d * m
        m -= 1

    return res
