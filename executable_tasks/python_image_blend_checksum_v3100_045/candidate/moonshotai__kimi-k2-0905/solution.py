def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)          # pre-allocate once
    src_mv = memoryview(src).cast('B')
    dst_mv = memoryview(dst).cast('B')
    out_mv = memoryview(out).cast('B')

    h = 1469598103934665603              # FNV-1a offset basis
    prime = 1099511628211
    mask = (1 << 64) - 1

    for _ in range(iters):
        # reset output buffer (cheap memset)
        for i in range(pixels * 4):
            out_mv[i] = 0

        o = 0
        for i in range(0, pixels * 4, 4):
            a = src_mv[i + 3]
            r = blend_byte(src_mv[i],     dst_mv[i],     a)
            g = blend_byte(src_mv[i + 1], dst_mv[i + 1], a)
            b = blend_byte(src_mv[i + 2], dst_mv[i + 2], a)

            out_mv[o]     = r
            out_mv[o + 1] = g
            out_mv[o + 2] = b
            out_mv[o + 3] = 255

            # inline checksum_bytes update
            h ^= r
            h = (h * prime) & mask
            h ^= g
            h = (h * prime) & mask
            h ^= b
            h = (h * prime) & mask
            h ^= 255
            h = (h * prime) & mask

            o += 4
    return h