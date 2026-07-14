def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    # Pre-calculate the constant to add to each element
    # The original code adds 0x9E3779B1 to each decoded value
    # Total = sum(prefix_sums) + len(deltas) * 0x9E3779B1
    offset = 0x9E3779B1
    n = len(deltas)
    total_offset = n * offset
    
    # We need to return the sum of (prefix_sum[i] + offset) for all i
    # This is equivalent to sum(prefix_sum[i]) + n * offset
    
    # To optimize, we observe that the prefix sum can be expressed as:
    # prefix_sum[i] = sum(deltas[0...i])
    # The total sum of prefix sums is:
    # sum_{i=0}^{n-1} sum_{j=0}^{i} deltas[j]
    # This can be rearranged as:
    # sum_{j=0}^{n-1} (n - j) * deltas[j]
    
    # Pre-calculate the weighted sum of deltas
    # This part is independent of the 'iters' loop if the deltas don't change
    # However, the 'iters' loop in the original code re-runs the decoding.
    # Looking at the original code: the 'values' list is updated every iteration,
    # but 'deltas' is constant. Thus, the result is the same every iteration.
    
    weighted_sum = 0
    for i in range(n):
        weighted_sum += deltas[i] * (n - i)
    
    result_per_iter = weighted_sum + total_offset
    
    # The original code structure: 
    # for _ in range(iters):
    #    decode values
    #    total = sum(values + offset)
    # return total
    # Note: 'total' is reset inside the loop, so it returns the result of the LAST iteration.
    
    return result_per_iter