MASK64 = 0xFFFFFFFFFFFFFFFF
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Reuse output storage across iterations.
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

            inv_a = 255 - a
            d0 = dst_mv[j]
            d1 = dst_mv[j + 1]
            d2 = dst_mv[j + 2]

            out_mv[j] = (s0 * a + d0 * inv_a + 127) // 255
            out_mv[j + 1] = (s1 * a + d1 * inv_a + 127) // 255
            out_mv[j + 2] = (s2 * a + d2 * inv_a + 127) // 255
            out_mv[j + 3] = 255
            j += 4

        h_local = FNV_OFFSET
        for b in out_mv:
            h_local ^= b
            h_local = (h_local * FNV_PRIME) & MASK64
        h = h_local

    return h
