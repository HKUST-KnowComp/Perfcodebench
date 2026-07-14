def run(ids, dict_vals, iters: int) -> int:
    vals = dict_vals
    ids_local = ids
    xor_add = 1
    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    h = 0
    for _ in range(iters):
        hh = fnv_offset
        for idx in ids_local:
            hh ^= vals[idx] + xor_add
            hh = (hh * fnv_prime) & mask
        h = hh
    return h
