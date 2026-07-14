def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    total = width * height
    # local references for speed
    src_loc = src
    dst_loc = dst
    FNVA_OFFSET = 1469598103934665603
    FNVA_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = FNVA_OFFSET
        # iterate over pixel starts (0,4,8,...)
        for idx in range(0, total * 4, 4):
            a = src_loc[idx + 3]
            a_inv = 255 - a
            # red
            b = (src_loc[idx] * a + dst_loc[idx] * a_inv + 127) // 255
            h ^= b
            h = (h * FNVA_PRIME) & MASK64
            # green
            b = (src_loc[idx + 1] * a + dst_loc[idx + 1] * a_inv + 127) // 255
            h ^= b
            h = (h * FNVA_PRIME) & MASK64
            # blue
            b = (src_loc[idx + 2] * a + dst_loc[idx + 2] * a_inv + 127) // 255
            h ^= b
            h = (h * FNVA_PRIME) & MASK64
            # alpha (always 255)
            h ^= 255
            h = (h * FNVA_PRIME) & MASK64
    return h