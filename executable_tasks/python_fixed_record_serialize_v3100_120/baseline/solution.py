def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    out = bytearray()
    h = 0
    for _ in range(iters):
        out.clear()
        for i in range(len(ids)):
            out.extend(int(ids[i]).to_bytes(4, "little"))
            out.extend(int(ts[i]).to_bytes(4, "little"))
            out.extend(int(levels[i]).to_bytes(2, "little"))
            out.extend(int(lens[i]).to_bytes(2, "little"))
        h = checksum_bytes(out)
    return h
