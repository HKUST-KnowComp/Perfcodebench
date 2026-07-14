def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0

    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    num_pixels = width * height
    src_local = src
    dst_local = dst

    for base in range(0, num_pixels * 4, 4):
        a = src_local[base + 3]
        inv_a = 255 - a

        # Red
        s = src_local[base]
        d = dst_local[base]
        r = ((s * a) + (d * inv_a) + 127) // 255

        # Green
        s = src_local[base + 1]
        d = dst_local[base + 1]
        g = ((s * a) + (d * inv_a) + 127) // 255

        # Blue
        s = src_local[base + 2]
        d = dst_local[base + 2]
        b = ((s * a) + (d * inv_a) + 127) // 255

        h ^= r
        h = (h * MULT) & MASK
        h ^= g
        h = (h * MULT) & MASK
        h ^= b
        h = (h * MULT) & MASK
        h ^= 255
        h = (h * MULT) & MASK

    return h
