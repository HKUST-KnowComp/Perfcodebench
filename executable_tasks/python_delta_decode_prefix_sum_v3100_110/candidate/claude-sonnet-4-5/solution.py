def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    constant = 0x9E3779B1
    
    for _ in range(iters):
        total = 0
        prefix = 0
        
        for i, d in enumerate(deltas):
            prefix += d
            total += prefix + constant
    
    return total
