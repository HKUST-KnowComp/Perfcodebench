def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    prime = 1099511628211
    mask = (1 << 64) - 1

    src_b = src
    dst_b = dst

    h = 0
    # Process iters times; checksum corresponds to the last iteration as in baseline
    for _ in range(iters):
        h_iter = 1469598103934665603
        i = 0
        end = pixels * 4
        while i < end:
            a = src_b[i + 3]

            # R
            s = src_b[i]
            d = dst_b[i]
            t = ((d << 8) - d) + (s - d) * a + 127
            v = t // 255
            h_iter ^= v
            h_iter = (h_iter * prime) & mask

            # G
            s = src_b[i + 1]
            d = dst_b[i + 1]
            t = ((d << 8) - d) + (s - d) * a + 127
            v = t // 255
            h_iter ^= v
            h_iter = (h_iter * prime) & mask

            # B
            s = src_b[i + 2]
            d = dst_b[i + 2]
            t = ((d << 8) - d) + (s - d) * a + 127
            v = t // 255
            h_iter ^= v
            h_iter = (h_iter * prime) & mask

            # A fixed to 255
            v = 255
            h_iter ^= v
            h_iter = (h_iter * prime) & mask

            i += 4
        h = h_iter
    return h
