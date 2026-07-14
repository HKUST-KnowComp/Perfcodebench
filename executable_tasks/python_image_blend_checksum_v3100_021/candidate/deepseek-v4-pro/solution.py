def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0
    pixels = width * height
    out_h = 0
    mask = (1 << 64) - 1
    mul = 1099511628211
    start_h = 1469598103934665603
    for _ in range(iters):
        h = start_h
        for base in range(0, pixels * 4, 4):
            s0 = src[base]
            s1 = src[base + 1]
            s2 = src[base + 2]
            a = src[base + 3]
            d0 = dst[base]
            d1 = dst[base + 1]
            d2 = dst[base + 2]
            t = 255 - a
            r = ((s0 * a) + (d0 * t) + 127) // 255
            g = ((s1 * a) + (d1 * t) + 127) // 255
            b = ((s2 * a) + (d2 * t) + 127) // 255
            h ^= r
            h = (h * mul) & mask
            h ^= g
            h = (h * mul) & mask
            h ^= b
            h = (h * mul) & mask
            h ^= 255
            h = (h * mul) & mask
        out_h = h
    return out_h
