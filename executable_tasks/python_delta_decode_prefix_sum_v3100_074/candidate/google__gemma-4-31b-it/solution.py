def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = d0 + (d0 + d1) + (d0 + d1 + d2) ...
    # S = n*d0 + (n-1)*d1 + (n-2)*d2 + ... + 1*dn-1
    # This can be computed in O(N) instead of O(iters * N).
    
    weighted_sum = 0
    for i, d in enumerate(deltas):
        weighted_sum += (n - i) * d
    
    # Each iteration adds the weighted_sum and the constant offset for each element
    # Constant offset per iteration: n * 0x9E3779B1
    total_per_iter = weighted_sum + n * 0x9E3779B1
    
    return total_per_iter * iters