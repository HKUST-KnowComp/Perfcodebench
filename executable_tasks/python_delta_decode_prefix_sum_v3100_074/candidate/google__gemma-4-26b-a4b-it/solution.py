def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    # Pre-calculate the constant to add to each element
    # The original code adds 0x9E3779B1 to every decoded value.
    # Total = sum(prefix_sums) + len(deltas) * 0x9E3779B1
    constant_offset = 0x9E3779B1
    n = len(deltas)
    total_offset = n * constant_offset
    
    # We need to return the sum of (prefix_sum[i] + constant_offset)
    # across all i, repeated 'iters' times.
    # However, the original code resets 'total' inside the 'iters' loop.
    # Wait, looking at the baseline: 
    # for _ in range(iters):
    #    ... calculate values ...
    #    total = 0
    #    for v in values: total += v + offset
    # This means 'total' is overwritten every iteration. 
    # The final 'total' returned is just the result of the LAST iteration.
    
    # Let's re-examine the baseline logic carefully:
    # for _ in range(iters):
    #     current = 0
    #     for i, d in enumerate(deltas):
    #         current += d
    #         values[i] = current
    #     total = 0
    #     for v in values:
    #         total += v + 0x9E3779B1
    # return total
    
    # Since 'total' is reset every iteration and 'deltas' is constant,
    # the result is the same for every iteration. We only need to run it once.
    
    total = total_offset
    current = 0
    for d in deltas:
        current += d
        total += current
        
    return total