import struct

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    n = len(ids)
    rec_size = 12                       # 4+4+2+2
    buf = bytearray(n * rec_size)       # pre-allocate once

    h = 0
    for _ in range(iters):
        # pack all records into the buffer
        off = 0
        for i in range(n):
            struct.pack_into('<IIHH', buf, off,
                             int(ids[i]), int(ts[i]),
                             int(levels[i]), int(lens[i]))
            off += rec_size

        # compute checksum on the populated buffer
        h = checksum_bytes(buf)
    return h