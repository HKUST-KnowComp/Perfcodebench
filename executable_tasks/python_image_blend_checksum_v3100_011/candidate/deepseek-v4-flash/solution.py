def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    h = 0
    for _ in range(iters):
        hs = FNV_OFFSET
        for i in range(pixels):
            base = i << 2
            a = src[base + 3]
            if a == 255:
                hs ^= src[base]
                hs = (hs * FNV_PRIME) & MASK
                hs ^= src[base + 1]
                hs = (hs * FNV_PRIME) & MASK
                hs ^= src[base + 2]
                hs = (hs * FNV_PRIME) & MASK
                hs ^= 255
                hs = (hs * FNV_PRIME) & MASK
            else:
                n_a = 255 - a
                add127 = 127
                s0 = src[base]; d0 = dst[base]
                b0 = (s0 * a + d0 * n_a + add127) // 255
                hs ^= b0
                hs = (hs * FNV_PRIME) & MASK
                s1 = src[base + 1]; d1 = dst[base + 1]
                b1 = (s1 * a + d1 * n_a + add127) // 255
                hs ^= b1
                hs = (hs * FNV_PRIME) & MASK
                s2 = src[base + 2]; d2 = dst[base + 2]
                b2 = (s2 * a + d2 * n_a + add127) // 255
                hs ^= b2
                hs = (hs * FNV_PRIME) & MASK
                hs ^= 255
                hs = (hs * FNV_PRIME) & MASK
        h = hs
    return h
