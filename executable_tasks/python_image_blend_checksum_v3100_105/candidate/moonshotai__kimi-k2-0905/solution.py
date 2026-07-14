def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    h = 1469598103934665603
    # stable, fast FNV-1a multiplier
    FNV = 1099511628211
    mask = (1 << 64) - 1

    # memoryview: zero-copy, C-level access to the buffers
    sv = memoryview(src).cast('B')
    dv = memoryview(dst).cast('B')

    for _ in range(iters):
        h = 1469598103934665603  # reset FNV state each iteration
        # walk pixels once, blend + checksum on the fly
        for i in range(pixels):
            base = i * 4
            a = sv[base + 3]
            r = blend_byte(sv[base], dv[base], a)
            g = blend_byte(sv[base + 1], dv[base + 1], a)
            b = blend_byte(sv[base + 2], dv[base + 2], a)

            # update checksum with the four output bytes
            h ^= r
            h = (h * FNV) & mask
            h ^= g
            h = (h * FNV) & mask
            h ^= b
            h = (h * FNV) & mask
            h ^= 255
            h = (h * FNV) & mask

    return h