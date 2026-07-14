MASK64 = (1 << 64) - 1
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
        # Blend directly into the preallocated output buffer.
        j = 0
        for _ in range(pixels):
            a = src_mv[j + 3]
            inv = 255 - a

            out_mv[j] = (src_mv[j] * a + dst_mv[j] * inv + 127) // 255
            j1 = j + 1
            out_mv[j1] = (src_mv[j1] * a + dst_mv[j1] * inv + 127) // 255
            j2 = j + 2
            out_mv[j2] = (src_mv[j2] * a + dst_mv[j2] * inv + 127) // 255
            out_mv[j + 3] = 255
            j += 4

        # Compute checksum in-place over the output bytes.
        hh = FNV_OFFSET
        for b in out_mv:
            hh ^= b
            hh = (hh * FNV_PRIME) & MASK64
        h = hh

    return h
