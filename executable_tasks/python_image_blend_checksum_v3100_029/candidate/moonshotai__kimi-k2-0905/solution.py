def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    src_mv = memoryview(src).cast('B')
    dst_mv = memoryview(dst).cast('B')

    # FNV-1a 64-bit constants
    FNV_OFFSET = 14695981039346656037
    FNV_PRIME  = 1099511628211

    h = 0
    for _ in range(iters):
        h = FNV_OFFSET
        # Build output and hash in one pass, no intermediate bytearray
        for i in range(pixels):
            base = i * 4
            a = src_mv[base + 3]
            r = blend_byte(src_mv[base],     dst_mv[base],     a)
            g = blend_byte(src_mv[base + 1], dst_mv[base + 1], a)
            b = blend_byte(src_mv[base + 2], dst_mv[base + 2], a)

            # Update hash directly on the four output bytes
            h = (h ^ r) * FNV_PRIME & 0xFFFFFFFFFFFFFFFF
            h = (h ^ g) * FNV_PRIME & 0xFFFFFFFFFFFFFFFF
            h = (h ^ b) * FNV_PRIME & 0xFFFFFFFFFFFFFFFF
            h = (h ^ 255) * FNV_PRIME & 0xFFFFFFFFFFFFFFFF
    return h