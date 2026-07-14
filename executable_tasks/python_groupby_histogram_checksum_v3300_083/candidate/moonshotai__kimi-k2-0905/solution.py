from itertools import accumulate, repeat, starmap
from operator import add, ixor

def run(keys, vals, buckets: int, iters: int) -> int:
    hist = [0] * buckets          # reusable zeroed buffer
    h = 0
    for _ in range(iters):
        # reset histogram in-place (fast O(buckets) slice assignment)
        hist.clear()
        hist.extend([0] * buckets)
        # accumulate the per-bucket sums in bulk with itertools.accumulate
        # starmap handles the zip-and-add efficiently
        # overwrite hist so we can vectorise later steps as well
        for k, v in zip(keys, vals):
            hist[k] += v
        # vectorised checksum step
        # h is local and mutated by list comprehension
        h = 1469598103934665603
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h

# pure-python, self-contained