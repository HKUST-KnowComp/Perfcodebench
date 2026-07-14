import numpy as np

_FNV64_OFFSET = np.uint64(14695981039346656037)
_FNV64_PRIME  = np.uint64(1099511628211)

_CHUNK = 8192

@np.errstate(all='ignore')
def run(scores: list[int], k: int, iters: int) -> int:
    # convert once; the list is not mutated
    scores_arr = np.frombuffer(bytearray(scores), dtype=np.int32).copy()
    n = scores_arr.size
    h = np.uint64(14695981039346656037)

    scratch = np.empty(k, dtype=np.int32)

    for _ in range(iters):
        # indices of the k largest values
        k_idx = np.argpartition(-scores_arr, k)[:k]

        # order them by descending score, then by ascending original index
        k_scores = scores_arr[k_idx]
        order = np.lexsort((k_idx, -k_scores))
        top_k_idx = k_idx[order]

        h_cur = _FNV64_OFFSET

        # hash everything in large chunks to stay in C for longer
        for start in range(0, k, _CHUNK):
            end = min(start + _CHUNK, k)
            chunk_idx = top_k_idx[start:end]
            chunk_val = scores_arr[chunk_idx]
            # combine into a single 64-bit quantity:  (score << 32) | index
            combined = np.bitwise_or(
                np.left_shift(chunk_val.astype(np.uint64), 32),
                chunk_idx.astype(np.uint64)
            )
            # vectorised FNV-1a update
            for x in combined:
                h_cur ^= x
                h_cur = (h_cur * _FNV64_PRIME) & np.uint64(0xFFFFFFFFFFFFFFFF)

        h = h_cur

    return int(h)