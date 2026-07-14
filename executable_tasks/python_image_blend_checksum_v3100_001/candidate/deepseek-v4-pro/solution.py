def run(src, dst, width, height, iters):
    h = 1469598103934665603
    mask = (1 << 64) - 1
    M = 1099511628211
    pixels = width * height
    for i in range(pixels):
        base = i * 4
        a = src[base + 3]
        inv_a = 255 - a
        # R
        out = (src[base] * a + dst[base] * inv_a + 127) // 255
        h ^= out
        h = (h * M) & mask
        # G
        out = (src[base + 1] * a + dst[base + 1] * inv_a + 127) // 255
        h ^= out
        h = (h * M) & mask
        # B
        out = (src[base + 2] * a + dst[base + 2] * inv_a + 127) // 255
        h ^= out
        h = (h * M) & mask
        # A
        h ^= 255
        h = (h * M) & mask
    return h
