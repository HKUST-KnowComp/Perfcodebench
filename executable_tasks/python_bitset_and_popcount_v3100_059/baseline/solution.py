def run(left: list[int], right: list[int], iters: int) -> int:
    bits_arr = [0] * (len(left) * 64)
    total = 0
    for _ in range(iters):
        for i, (a, b) in enumerate(zip(left, right)):
            word = a & b
            base = i * 64
            for bit in range(64):
                bits_arr[base + bit] = (word >> bit) & 1
        total = 0
        for v in bits_arr:
            total += v
    return total
