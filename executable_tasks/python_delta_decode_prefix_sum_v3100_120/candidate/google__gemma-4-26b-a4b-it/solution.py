def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    n = len(deltas)
    prefix_sums = [0] * n
    current = 0
    for i in range(n):
        current += deltas[i]
        prefix_sums[i] = current
    
    # The original code does: 
    # for _ in range(iters):
    #    recalculate prefix_sums (which are constant)
    #    total = sum(v + offset for v in prefix_sums)
    # This is equivalent to: iters * sum(v + offset for v in prefix_sums)
    
    offset = 0x9E3779B1
    single_iter_total = 0
    for v in prefix_sums:
        single_iter_total += (v + offset)
        
    return single_iter_total * iters