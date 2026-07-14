def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Pre-zip to avoid repeated overhead in the loop
    pairs = list(zip(left, right))
    
    for _ in range(iters):
        current_total = 0
        for a, b in pairs:
            # int.bit_count() is available in Python 3.10+
            # and is highly optimized for performance.
            current_total += (a & b).bit_count()
        total = current_total
    return total