def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    src_loc = src
    dst_loc = dst
    init = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    h_final = 0
    for _ in range(iters):
        h = init
        for i in range(pixels):
            base = i * 4
            a = src_loc[base + 3]
            inv_a = 255 - a
            # blend red
            r = (src_loc[base] * a + dst_loc[base] * inv_a + 127) // 255
            h = (h ^ r) * mult & mask
            # blend green
            g = (src_loc[base + 1] * a + dst_loc[base + 1] * inv_a + 127) // 255
            h = (h ^ g) * mult & mask
            # blend blue
            b = (src_loc[base + 2] * a + dst_loc[base + 2] * inv_a + 127) // 255
            h = (h ^ b) * mult & mask
            # alpha always 255
            h = (h ^ 255) * mult & mask
        h_final = h
    return h_final