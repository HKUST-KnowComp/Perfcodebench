def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    constant_sum = n * 0x9E3779B1
    
    for _ in range(iters):
        total = constant_sum
        current = 0
        for i, d in enumerate(deltas):
            current += d
            total += current
    
    return total
