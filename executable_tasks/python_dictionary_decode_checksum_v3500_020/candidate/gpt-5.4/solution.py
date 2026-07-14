def run(ids, dict_vals, iters: int) -> int:
    vals = dict_vals
    ids_local = ids
    xor_const = 1469598103934665603
    mul = 1099511628211
    mask = (1 << 64) - 1

    h = 0
    for _ in range(iters):
        h = xor_const
        for idx in ids_local:
            h ^= vals[idx] + 1
            h = (h * mul) & mask
    return h
