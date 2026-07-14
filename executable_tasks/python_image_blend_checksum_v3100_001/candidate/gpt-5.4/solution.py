MASK64 = (1 << 64) - 1


def checksum_bytes(data: bytearray) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & MASK64
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Reuse a single output buffer across iterations to avoid repeated growth.
    out = bytearray(n)

    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    h = 0
    for _ in range(iters):
        j = 0
        for _ in range(pixels):
            s0 = src_mv[j]
            s1 = src_mv[j + 1]
            s2 = src_mv[j + 2]
            a = src_mv[j + 3]
            inv = 255 - a

            out_mv[j] = (s0 * a + dst_mv[j] * inv + 127) // 255
            out_mv[j + 1] = (s1 * a + dst_mv[j + 1] * inv + 127) // 255
            out_mv[j + 2] = (s2 * a + dst_mv[j + 2] * inv + 127) // 255
            out_mv[j + 3] = 255
            j += 4

        h = checksum_bytes(out)

    return h
