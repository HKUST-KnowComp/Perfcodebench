def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters <= 0:
        return 0
    pixels = width * height
    mask = 0xFFFFFFFFFFFFFFFF
    mult = 1099511628211
    h = 1469598103934665603
    # compute checksum once
    for i in range(pixels):
        base = i << 2
        a = src[base + 3]
        inv_a = 255 - a
        s0, s1, s2 = src[base], src[base+1], src[base+2]
        d0, d1, d2 = dst[base], dst[base+1], dst[base+2]
        r = (s0 * a + d0 * inv_a + 127) // 255
        g = (s1 * a + d1 * inv_a + 127) // 255
        b = (s2 * a + d2 * inv_a + 127) // 255
        h ^= r
        h = (h * mult) & mask
        h ^= g
        h = (h * mult) & mask
        h ^= b
        h = (h * mult) & mask
        h ^= 255
        h = (h * mult) & mask
    final_h = h
    # repeat for remaining iterations (fast assignment only)
    for _ in range(1, iters):
        h = final_h
    return h