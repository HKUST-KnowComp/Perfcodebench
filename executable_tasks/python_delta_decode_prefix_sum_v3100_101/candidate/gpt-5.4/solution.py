def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    bias = 0x9E3779B1
    total = 0

    # Fuse delta decoding and aggregation to avoid the intermediate list
    # and the second pass. Hoist lookups to locals for faster access.
    for _ in range(iters):
        current = 0
        acc = 0
        for d in deltas:
            current += d
            acc += current
        total = acc + n * bias
    return total
