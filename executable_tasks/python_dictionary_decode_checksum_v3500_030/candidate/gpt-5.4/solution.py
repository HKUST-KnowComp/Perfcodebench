def run(ids, dict_vals, iters: int) -> int:
    # Decode and checksum in a single pass to avoid materializing the decoded list.
    # Keep frequently used values in locals to reduce global/builtin lookups.
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603

    ids_local = ids
    dict_local = dict_vals

    h = 0
    for _ in range(iters):
        h = offset
        for idx in ids_local:
            h ^= dict_local[idx] + 1
            h = (h * prime) & mask
    return h
