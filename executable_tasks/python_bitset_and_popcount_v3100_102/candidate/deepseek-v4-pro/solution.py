def run(left: list[int], right: list[int], iters: int) -> int:
    # Precompute the bitwise AND of corresponding words
    words = [a & b for a, b in zip(left, right)]
    # Local reference to the bit_count method for speed
    bit_count = int.bit_count
    total = 0
    for _ in range(iters):
        # Fast C-level sum of popcounts: map applies bit_count in C, sum reduces
        total = sum(map(bit_count, words))
    return total
