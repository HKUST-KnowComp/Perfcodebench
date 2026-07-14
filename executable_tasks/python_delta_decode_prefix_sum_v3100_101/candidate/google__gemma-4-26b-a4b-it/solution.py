def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # Pre-calculate the contribution of each delta to the prefix sum total.
    # The prefix sum at index i is sum(deltas[0...i]).
    # The total sum of prefix sums is sum_{i=0}^{n-1} (n-i) * deltas[i].
    # Additionally, we must account for the constant 0x9E3779B1 added to each decoded value.
    
    prefix_sum_total = 0
    for i in range(n):
        prefix_sum_total += deltas[i] * (n - i)
    
    # The constant part: each of the n elements has 0x9E3779B1 added to it.
    constant_offset = n * 0x9E3779B1
    
    # The total for one iteration
    iteration_total = prefix_sum_total + constant_offset
    
    # Since the deltas and the process are deterministic and don't change between iters,
    # we can simply multiply the single iteration result by iters.
    return iteration_total * iters