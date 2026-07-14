def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0x9E3779B1 * n * iters if iters > 0 else 0
    
    # Precompute the sum of prefix sums using closed-form:
    # prefix[i] = sum(deltas[0:i+1])
    # sum(prefix) = sum over i of (n - i) * deltas[i]
    prefix_sum_total = 0
    current = 0
    for d in deltas:
        current += d
        prefix_sum_total += current
    
    # Each iteration adds: sum(prefix) + n * 0x9E3779B1
    magic = 0x9E3779B1
    single_iter_total = prefix_sum_total + n * magic
    
    return single_iter_total * iters
