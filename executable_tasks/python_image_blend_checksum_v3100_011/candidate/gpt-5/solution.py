def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Streamed blending and checksum without building an output buffer.
    pixels = width * height
    n = pixels * 4

    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211
    mask64 = (1 << 64) - 1
    p255 = 255

    h_last = 0

    # Local aliases for speed
    src_b = src
    dst_b = dst

    for _ in range(iters):
        h = fnv_offset
        base = 0
        while base < n:
            a = src_b[base + 3]
            if a == 0:
                # Output equals destination RGB, alpha 255
                b0 = dst_b[base]
                h ^= b0
                h = (h * fnv_prime) & mask64
                b1 = dst_b[base + 1]
                h ^= b1
                h = (h * fnv_prime) & mask64
                b2 = dst_b[base + 2]
                h ^= b2
                h = (h * fnv_prime) & mask64
            elif a == 255:
                # Output equals source RGB, alpha 255
                b0 = src_b[base]
                h ^= b0
                h = (h * fnv_prime) & mask64
                b1 = src_b[base + 1]
                h ^= b1
                h = (h * fnv_prime) & mask64
                b2 = src_b[base + 2]
                h ^= b2
                h = (h * fnv_prime) & mask64
            else:
                inva = p255 - a
                # R
                s = src_b[base]
                d = dst_b[base]
                b0 = (s * a + d * inva + 127) // 255
                h ^= b0
                h = (h * fnv_prime) & mask64
                # G
                s = src_b[base + 1]
                d = dst_b[base + 1]
                b1 = (s * a + d * inva + 127) // 255
                h ^= b1
                h = (h * fnv_prime) & mask64
                # B
                s = src_b[base + 2]
                d = dst_b[base + 2]
                b2 = (s * a + d * inva + 127) // 255
                h ^= b2
                h = (h * fnv_prime) & mask64
            # Alpha is always 255 in output
            h ^= p255
            h = (h * fnv_prime) & mask64
            base += 4
        h_last = h

    return h_last
