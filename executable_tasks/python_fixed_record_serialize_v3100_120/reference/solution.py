def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    out = bytearray(len(ids) * 12)
    h = 0
    for _ in range(iters):
        ptr = 0
        for i in range(len(ids)):
            out[ptr:ptr+4] = int(ids[i]).to_bytes(4, "little"); ptr += 4
            out[ptr:ptr+4] = int(ts[i]).to_bytes(4, "little"); ptr += 4
            out[ptr:ptr+2] = int(levels[i]).to_bytes(2, "little"); ptr += 2
            out[ptr:ptr+2] = int(lens[i]).to_bytes(2, "little"); ptr += 2
        h = checksum_bytes(out)
    return h
