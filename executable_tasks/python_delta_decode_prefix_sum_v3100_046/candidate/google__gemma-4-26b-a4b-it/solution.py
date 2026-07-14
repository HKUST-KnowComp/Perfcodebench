def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # Pre-calculate the contribution of each delta to the final sum.
    # The prefix sum at index i is sum(deltas[0...i]).
    # The total sum of prefix sums is sum_{i=0}^{n-1} sum_{j=0}^{i} deltas[j].
    # This can be rearranged as sum_{j=0}^{n-1} deltas[j] * (n - j).
    # Additionally, the original code adds 0x9E3779B1 to each of the n prefix values.
    
    # Pre-calculate the base sum of prefix sums
    base_prefix_sum = 0
    for i in range(n):
        base_prefix_sum += deltas[i] * (n - i)
    
    # The constant part: n * 0x9E3779B1
    constant_offset = n * 0x9E3779B1
    
    # The total sum for one iteration
    iteration_total = base_prefix_sum + constant_offset
    
    return iteration_total * iters