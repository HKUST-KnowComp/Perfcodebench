def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0

    pixels = width * height
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MUL = 1099511628211

    for i in range(pixels):
        base = i * 4
        a = src[base + 3]
        if a == 0:
            # early exit: blend_byte returns dst directly
            r = dst[base]
            g = dst[base + 1]
            b = dst[base + 2]
        elif a == 255:
            r = src[base]
            g = src[base + 1]
            b = src[base + 2]
        else:
            inv_a = 255 - a
            # Inlined blend: ((s*a) + (d*inv_a) + 127) // 255
            r = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            g = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            b = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255

        # update checksum with r, g, b, 255
        h ^= r
        h = (h * MUL) & MASK
        h ^= g
        h = (h * MUL) & MASK
        h ^= b
        h = (h * MUL) & MASK
        h ^= 255
        h = (h * MUL) & MASK

    return h
