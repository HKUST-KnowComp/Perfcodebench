def run(ids, dict_vals, iters: int) -> int:
    # Cache frequently used globals/objects in locals for faster access.
    ids_local = ids
    vals = dict_vals
    n = len(ids_local)

    # Precompute the decoded values once since ids and dict_vals do not change
    # across iterations. The baseline redundantly decodes on every iteration.
    decoded = [0] * n
    for i in range(n):
        decoded[i] = vals[ids_local[i]]

    # FNV-1a style checksum constants.
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603

    h = 0
    for _ in range(iters):
        h = offset
        for v in decoded:
            h ^= v + 1
            h = (h * prime) & mask
    return h
