MASK64 = (1 << 64) - 1


def checksum_bytes(data: bytearray) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & MASK64
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    n = width * height * 4
    out = bytearray(n)

    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    h = 0
    for _ in range(iters):
        for base in range(0, n, 4):
            a = src_mv[base + 3]
            inv = 255 - a

            out_mv[base] = (src_mv[base] * a + dst_mv[base] * inv + 127) // 255
            out_mv[base + 1] = (src_mv[base + 1] * a + dst_mv[base + 1] * inv + 127) // 255
            out_mv[base + 2] = (src_mv[base + 2] * a + dst_mv[base + 2] * inv + 127) // 255
            out_mv[base + 3] = 255

        h = checksum_bytes(out)
    return h
