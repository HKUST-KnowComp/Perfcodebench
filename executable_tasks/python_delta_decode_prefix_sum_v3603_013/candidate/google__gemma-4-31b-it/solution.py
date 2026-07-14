def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = d0 + (d0 + d1) + (d0 + d1 + d2) ...
    # S = d0*n + d1*(n-1) + d2*(n-2) + ... + dn-1*1
    
    prefix_sum_of_sums = 0
    for i, d in enumerate(deltas):
        prefix_sum_of_sums += d * (n - i)
    
    # Each iteration adds the prefix sum of sums plus (0x9E3779B1 * n)
    constant_offset = 0x9E3779B1 * n
    total_per_iter = prefix_sum_of_sums + constant_offset
    
    return total_per_iter * iters