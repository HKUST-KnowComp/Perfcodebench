import os

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    src_view = memoryview(src)
    dst_view = memoryview(dst)

    # Pre-allocate output buffer once
    out = bytearray(pixels * 4)
    out_view = memoryview(out)

    h = 0
    for _ in range(iters):
        # FNV-1a 64-bit parameters
        fnv_offset = 1469598103934665603
        fnv_prime  = 1099511628211
        mask64 = (1 << 64) - 1

        h_iter = fnv_offset

        # Fast native loop over pixels
        for i in range(pixels):
            base = i << 2
            a = src_view[base + 3]
            inv_a = 255 - a

            r = ((src_view[base]     * a + dst_view[base]     * inv_a + 127) // 255)
            g = ((src_view[base + 1] * a + dst_view[base + 1] * inv_a + 127) // 255)
            b = ((src_view[base + 2] * a + dst_view[base + 2] * inv_a + 127) // 255)

            # Store and hash in one pass
            out_view[base]     = r
            out_view[base + 1] = g
            out_view[base + 2] = b
            out_view[base + 3] = 255

            # Update checksum incrementally
            h_iter ^= r
            h_iter = (h_iter * fnv_prime) & mask64
            h_iter ^= g
            h_iter = (h_iter * fnv_prime) & mask64
            h_iter ^= b
            h_iter = (h_iter * fnv_prime) & mask64
            h_iter ^= 255
            h_iter = (h_iter * fnv_prime) & mask64

        h = h_iter
    return h
