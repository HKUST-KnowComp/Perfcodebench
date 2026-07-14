def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Compute checksum directly while blending, avoiding temporary image allocation.
    pixels = width * height
    total_bytes = pixels * 4

    PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    OFFSET = 1469598103934665603

    src_b = src
    dst_b = dst

    h = 0
    for _ in range(iters):
        hh = OFFSET
        i = 0
        # Process 4 bytes per pixel: R, G, B, A
        while i < total_bytes:
            a = src_b[i + 3]
            inv = 255 - a

            # R
            val = src_b[i] * a + dst_b[i] * inv + 127
            b = val // 255
            hh = ((hh ^ b) * PRIME) & MASK

            # G
            val = src_b[i + 1] * a + dst_b[i + 1] * inv + 127
            b = val // 255
            hh = ((hh ^ b) * PRIME) & MASK

            # B
            val = src_b[i + 2] * a + dst_b[i + 2] * inv + 127
            b = val // 255
            hh = ((hh ^ b) * PRIME) & MASK

            # A is set to 255
            hh = ((hh ^ 255) * PRIME) & MASK

            i += 4
        h = hh
    return h
