def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    
    n = len(deltas)
    offset = n * 0x9E3779B1
    total_sum = 0
    
    for _ in range(iters):
        current = 0
        current_total = 0
        for d in deltas:
            current += d
            current_total += current
        total_sum += current_total + offset
        
    return total_sum