def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    # S = sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    single_iter_prefix_sum = 0
    for i, d in enumerate(deltas):
        single_iter_prefix_sum += d * (n - i)
    
    # Each element in the prefix sum list also has 0x9E3779B1 added to it
    # Total constant addition per iteration: n * 0x9E3779B1
    constant_sum = n * 0x9E3779B1
    
    return (single_iter_prefix_sum + constant_sum) * iters