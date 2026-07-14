def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels4 = width * height * 4
    out = bytearray(pixels4)
    mask = (1 << 64) - 1
    fnv_prime = 1099511628211
    h = 0

    src_b = src
    dst_b = dst
    out_b = out

    for _ in range(iters):
        i = 0
        while i < pixels4:
            a = src_b[i + 3]
            inv_a = 255 - a

            out_b[i] = (src_b[i] * a + dst_b[i] * inv_a + 127) // 255
            j = i + 1
            out_b[j] = (src_b[j] * a + dst_b[j] * inv_a + 127) // 255
            j = i + 2
            out_b[j] = (src_b[j] * a + dst_b[j] * inv_a + 127) // 255
            out_b[i + 3] = 255
            i += 4

        h_local = 1469598103934665603
        for b in out_b:
            h_local ^= b
            h_local = (h_local * fnv_prime) & mask
        h = h_local

    return h
