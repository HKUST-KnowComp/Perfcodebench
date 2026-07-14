import itertools

def run(deltas: list[int], iters: int) -> int:
    # Pre-allocate buffer only once
    values = [0] * len(deltas)
    const = 0x9E3779B1

    total = 0
    for _ in range(iters):
        # Decode deltas into prefix sums entirely in C
        acc = itertools.accumulate(deltas)
        for i, v in enumerate(acc):
            values[i] = v

        # Sum the values with the constant offset, again in C
        total = sum(v + const for v in values)

    return total