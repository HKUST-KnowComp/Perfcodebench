def run(deltas: list[int], iters: int) -> int:
    if iters <= 0 or not deltas:
        return 0
    
    current = 0
    total = 0
    offset = 0x9E3779B1
    
    for d in deltas:
        current += d
        total += current + offset
    
    return total