#!/usr/bin/env python3

def run(keys, vals, buckets: int, iters: int) -> int:
    # pre-allocate a re-usable bytearray for the histogram
    hist = bytearray(buckets * 8)
    mul = 1099511628211
    for _ in range(iters):
        hist[:] = b'\0' * (buckets * 8)
        for k, v in zip(keys, vals):
            # update hist[k] in-place as signed int64
            ofs = k * 8
            v1 = int.from_bytes(hist[ofs:ofs+8], byteorder='little', signed=True)
            v1 += int(v)
            hist[ofs:ofs+8] = v1.to_bytes(8, byteorder='little', signed=True)
        h = 1469598103934665603
        # compute checksum directly from the bytearray without converting to Python ints
        ofs = 0
        for _ in range(buckets):
            v1 = int.from_bytes(hist[ofs:ofs+8], byteorder='little', signed=True)
            h ^= v1 + 1
            h = (h * mul) & ((1 << 64) - 1)
            ofs += 8
    return h
