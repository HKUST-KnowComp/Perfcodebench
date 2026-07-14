def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    n = len(ids)
    total = n * 12
    out = bytearray(total)
    mv = memoryview(out)
    h = 0

    for _ in range(iters):
        pos = 0
        for i in range(n):
            mv[pos:pos + 4] = int(ids[i]).to_bytes(4, 'little')
            pos += 4
            mv[pos:pos + 4] = int(ts[i]).to_bytes(4, 'little')
            pos += 4
            mv[pos:pos + 2] = int(levels[i]).to_bytes(2, 'little')
            pos += 2
            mv[pos:pos + 2] = int(lens[i]).to_bytes(2, 'little')
            pos += 2
        h = checksum_bytes(out)
    return h
