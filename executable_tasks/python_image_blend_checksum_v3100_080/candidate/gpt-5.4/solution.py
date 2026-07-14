MASK64 = 0xFFFFFFFFFFFFFFFF
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Reuse a single output buffer across iterations.
    out = bytearray(n)

    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    h = 0
    for _ in range(iters):
        # Blend into preallocated output buffer.
        for base in range(0, n, 4):
            a = src_mv[base + 3]
            inv = 255 - a

            s = src_mv[base]
            d = dst_mv[base]
            out_mv[base] = (s * a + d * inv + 127) // 255

            s = src_mv[base + 1]
            d = dst_mv[base + 1]
            out_mv[base + 1] = (s * a + d * inv + 127) // 255

            s = src_mv[base + 2]
            d = dst_mv[base + 2]
            out_mv[base + 2] = (s * a + d * inv + 127) // 255

            out_mv[base + 3] = 255

        # Compute checksum directly over the output buffer.
        hh = FNV_OFFSET
        for b in out_mv:
            hh ^= b
            hh = (hh * FNV_PRIME) & MASK64
        h = hh

    return h
