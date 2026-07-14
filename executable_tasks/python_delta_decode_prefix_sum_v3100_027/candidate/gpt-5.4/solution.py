def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    bias = 0x9E3779B1

    # Fuse delta decoding and aggregation to avoid materializing the prefix array.
    # For each iteration, sum(prefixes) + n * bias.
    total = 0
    for _ in range(iters):
        current = 0
        s = 0
        for d in deltas:
            current += d
            s += current
        total = s + n * bias
    return total
