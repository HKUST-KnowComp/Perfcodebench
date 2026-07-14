def run(ids, dict_vals, iters: int) -> int:
    # Decode and checksum in a single pass to avoid allocating/storing
    # an intermediate decoded list. Bind frequently used values locally
    # to reduce global/name lookup overhead in the hot loop.
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211
    ids_local = ids
    dict_local = dict_vals

    h = 0
    for _ in range(iters):
        hh = offset
        for idx in ids_local:
            hh ^= dict_local[idx] + 1
            hh = (hh * prime) & mask
        h = hh
    return h
