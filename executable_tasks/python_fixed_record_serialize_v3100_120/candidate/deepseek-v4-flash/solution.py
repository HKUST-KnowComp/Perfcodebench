import struct

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    # Convert all fields to ints once
    ids = [int(x) for x in ids]
    ts = [int(x) for x in ts]
    levels = [int(x) for x in levels]
    lens = [int(x) for x in lens]
    n = len(ids)
    # Preallocate buffer
    buf = bytearray(n * 12)
    for i in range(n):
        off = i * 12
        struct.pack_into('<I', buf, off, ids[i])
        struct.pack_into('<I', buf, off + 4, ts[i])
        struct.pack_into('<H', buf, off + 8, levels[i])
        struct.pack_into('<H', buf, off + 10, lens[i])
    # Convert to bytes for fast constant-time hashing
    buf_bytes = bytes(buf)
    # Precompute mask for 64-bit
    mask = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        # Inline FNV-1a hash
        h = 1469598103934665603
        for b in buf_bytes:
            h ^= b
            h = (h * 1099511628211) & mask
    return h