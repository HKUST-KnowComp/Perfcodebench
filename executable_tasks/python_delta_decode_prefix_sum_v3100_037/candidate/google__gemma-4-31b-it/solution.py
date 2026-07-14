def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    
    # The sum of prefix sums is: 
    # S = sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    # S = sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    prefix_sum_total = 0
    for i, d in enumerate(deltas):
        prefix_sum_total += d * (n - i)
    
    # The original code adds 0x9E3779B1 to every element in the values list
    # for every iteration. There are n elements in the values list.
    constant_sum = n * 0x9E3779B1
    
    # Total = iters * (prefix_sum_total + constant_sum)
    return iters * (prefix_sum_total + constant_sum)