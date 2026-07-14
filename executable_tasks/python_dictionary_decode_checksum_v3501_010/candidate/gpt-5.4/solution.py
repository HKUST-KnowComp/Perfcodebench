def run(ids, dict_vals, iters: int) -> int:
    # Compute the checksum directly from decoded values without materializing
    # the intermediate decoded list. This preserves exact behavior because the
    # checksum depends only on the decoded sequence for each iteration, and the
    # sequence is identical every time.
    ids_local = ids
    dict_local = dict_vals
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603

    h = 0
    for _ in range(iters):
        h = offset
        for idx in ids_local:
            h ^= dict_local[idx] + 1
            h = (h * prime) & mask
    return h
