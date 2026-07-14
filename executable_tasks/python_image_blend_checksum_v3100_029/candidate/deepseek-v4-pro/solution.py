def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    seed = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for _ in range(iters):
        h = seed
        for i in range(pixels):
            base = i << 2
            a = src[base + 3]
            inv_a = 255 - a
            val = (src[base] * a + dst[base] * inv_a + 127) // 255
            h ^= val
            h = (h * mult) & mask
            val = (src[base + 1] * a + dst[base + 1] * inv_a + 127) // 255
            h ^= val
            h = (h * mult) & mask
            val = (src[base + 2] * a + dst[base + 2] * inv_a + 127) // 255
            h ^= val
            h = (h * mult) & mask
            h ^= 255
            h = (h * mult) & mask
    return h