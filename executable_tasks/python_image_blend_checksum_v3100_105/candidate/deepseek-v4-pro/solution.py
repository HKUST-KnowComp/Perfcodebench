def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    INIT = 1469598103934665603
    final_h = 0
    for _ in range(iters):
        h = INIT
        for i in range(0, pixels * 4, 4):
            a = src[i + 3]
            inv_a = 255 - a
            # Red
            val = (src[i] * a + dst[i] * inv_a + 127) // 255
            h ^= val
            h = (h * MULT) & MASK
            # Green
            val = (src[i + 1] * a + dst[i + 1] * inv_a + 127) // 255
            h ^= val
            h = (h * MULT) & MASK
            # Blue
            val = (src[i + 2] * a + dst[i + 2] * inv_a + 127) // 255
            h ^= val
            h = (h * MULT) & MASK
            # Alpha (always 255)
            h ^= 255
            h = (h * MULT) & MASK
        final_h = h
    return final_h
