def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    max_offset = pixels * 4
    mask64 = (1 << 64) - 1
    mul = 1099511628211
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for base in range(0, max_offset, 4):
            a = src[base + 3]
            # Red
            v = ((src[base] * a) + (dst[base] * (255 - a)) + 127) // 255
            h ^= v
            h = (h * mul) & mask64
            # Green
            v = ((src[base + 1] * a) + (dst[base + 1] * (255 - a)) + 127) // 255
            h ^= v
            h = (h * mul) & mask64
            # Blue
            v = ((src[base + 2] * a) + (dst[base + 2] * (255 - a)) + 127) // 255
            h ^= v
            h = (h * mul) & mask64
            # Alpha constant 255
            h ^= 255
            h = (h * mul) & mask64
    return h
