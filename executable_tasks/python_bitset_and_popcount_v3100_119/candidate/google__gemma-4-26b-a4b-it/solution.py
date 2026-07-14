def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Pre-zip to avoid repeated zip overhead in the loop
    pairs = list(zip(left, right))
    
    for _ in range(iters):
        total = 0
        for a, b in pairs:
            total += (a & b).bit_count()
    return total