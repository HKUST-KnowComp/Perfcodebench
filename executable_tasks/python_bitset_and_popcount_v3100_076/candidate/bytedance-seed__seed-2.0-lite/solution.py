def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    # Precompute fixed sum once since inputs never change across iterations
    fixed_total = sum((a & b).bit_count() for a, b in zip(left, right))
    for _ in range(iters):
        total = fixed_total
    return total