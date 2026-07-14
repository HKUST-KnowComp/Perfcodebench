def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    bias = 0x9E3779B1

    # Fuse delta decoding and aggregation to avoid materializing the
    # intermediate prefix array. The final result depends only on the last
    # iteration, matching the baseline behavior.
    total = 0
    for _ in range(iters):
        current = 0
        acc = 0
        for d in deltas:
            current += d
            acc += current
        total = acc + n * bias
    return total
