def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    src_mv = memoryview(src).cast('B')
    dst_mv = memoryview(dst).cast('B')
    out = bytearray(pixels * 4)          # pre-allocate once
    out_mv = memoryview(out).cast('B')

    h = 0
    for _ in range(iters):
        # Blend and checksum in one pass
        h = 1469598103934665603
        for i in range(pixels):
            base = i * 4
            a = src_mv[base + 3]
            r = blend_byte(src_mv[base],     dst_mv[base],     a)
            g = blend_byte(src_mv[base + 1], dst_mv[base + 1], a)
            b = blend_byte(src_mv[base + 2], dst_mv[base + 2], a)

            out_mv[base]     = r
            out_mv[base + 1] = g
            out_mv[base + 2] = b
            out_mv[base + 3] = 255

            # FNV-1a inline
            h ^= r
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            h ^= g
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            h ^= b
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            h ^= 255
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h