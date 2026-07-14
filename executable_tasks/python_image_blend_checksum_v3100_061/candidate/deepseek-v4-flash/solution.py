def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    src_loc = src
    dst_loc = dst
    offset_basis = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = offset_basis
        for i in range(pixels):
            base = i << 2
            a = src_loc[base + 3]
            inv_a = 255 - a
            # red
            b0 = ((src_loc[base] * a) + (dst_loc[base] * inv_a) + 127) // 255
            h ^= b0
            h = (h * prime) & mask
            # green
            b1 = ((src_loc[base + 1] * a) + (dst_loc[base + 1] * inv_a) + 127) // 255
            h ^= b1
            h = (h * prime) & mask
            # blue
            b2 = ((src_loc[base + 2] * a) + (dst_loc[base + 2] * inv_a) + 127) // 255
            h ^= b2
            h = (h * prime) & mask
            # alpha always 255
            h ^= 255
            h = (h * prime) & mask
    return h
