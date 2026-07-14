def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0
    pixels = width * height
    h = 1469598103934665603
    for i in range(pixels):
        base = i * 4
        a = src[base + 3]
        na = 255 - a
        # blend each channel
        r = (src[base] * a + dst[base] * na + 127) // 255
        g = (src[base + 1] * a + dst[base + 1] * na + 127) // 255
        b = (src[base + 2] * a + dst[base + 2] * na + 127) // 255
        # update hash with the four output bytes
        for byte in (r, g, b, 255):
            h ^= byte
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h
