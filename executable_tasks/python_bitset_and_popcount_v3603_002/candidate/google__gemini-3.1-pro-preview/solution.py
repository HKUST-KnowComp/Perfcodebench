def run(left: list[int], right: list[int], iters: int) -> int:
    """
    Optimized popcount of bitwise AND between two lists of integers.
    Uses the built-in bit_count() method for high-performance popcounting.
    """
    # Pre-zip the lists to avoid repeated zip overhead inside the loop
    pairs = list(zip(left, right))
    
    total = 0
    # The baseline resets total = 0 inside the loop and returns the result of the last iteration.
    # We maintain this behavior while optimizing the inner calculation.
    for _ in range(iters):
        total = sum((a & b).bit_count() for a, b in pairs)
        
    return total