def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Pre-zip to avoid overhead inside the loop
    pairs = list(zip(left, right))
    
    for _ in range(iters):
        total = 0
        for a, b in pairs:
            # bit_count() is the fastest way to perform popcount in Python 3.10+
            total += (a & b).bit_count()
    return total