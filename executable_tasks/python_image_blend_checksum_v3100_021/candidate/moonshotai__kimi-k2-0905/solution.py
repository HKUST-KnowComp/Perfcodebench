def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # stable FNV-1a constants
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME  = 1099511628211
    MASK64     = (1 << 64) - 1

    # zero-copy typed buffers
    sv = memoryview(src).cast('B')
    dv = memoryview(dst).cast('B')

    h = 0
    for _ in range(iters):
        # reset hash for this iteration
        local_h = FNV_OFFSET
        # walk pixels in one tight loop
        for i in range(pixels):
            base = i * 4
            a = sv[base + 3]
            # blend RGB channels
            r = blend_byte(sv[base],     dv[base],     a)
            g = blend_byte(sv[base + 1], dv[base + 1], a)
            b = blend_byte(sv[base + 2], dv[base + 2], a)
            # accumulate FNV-1a over the 4 output bytes
            local_h ^= r
            local_h = (local_h * FNV_PRIME) & MASK64
            local_h ^= g
            local_h = (local_h * FNV_PRIME) & MASK64
            local_h ^= b
            local_h = (local_h * FNV_PRIME) & MASK64
            local_h ^= 255
            local_h = (local_h * FNV_PRIME) & MASK64
        h = local_h
    return h