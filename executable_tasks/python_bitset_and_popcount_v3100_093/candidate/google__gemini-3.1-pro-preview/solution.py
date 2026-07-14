def run(left: list[int], right: list[int], iters: int) -> int:
    """
    Optimized popcount of bitwise AND between two lists of integers.
    Uses Python 3.10+ int.bit_count() for high-performance bit counting.
    """
    total = 0
    # Pre-calculate the bitwise ANDs to avoid redundant work inside the iters loop
    # if the benchmark allows, but to be safe and preserve the logic of the loop,
    # we perform the calculation inside the loop using the most efficient Python idioms.
    for _ in range(iters):
        total = sum((a & b).bit_count() for a, b in zip(left, right))
    return total