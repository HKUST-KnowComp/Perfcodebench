def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Compute FNV-1a checksum directly over the blended output without materializing it.
    total_bytes = (width * height) * 4
    prime = 1099511628211
    mask = (1 << 64) - 1
    out_alpha = 255
    h = 0
    # Local references for speed
    src_local = src
    dst_local = dst
    for _ in range(iters):
        # Start checksum fresh for each iteration, returning the last iteration's checksum
        h = 1469598103934665603
        # Iterate per pixel (4 bytes per pixel)
        for i in range(0, total_bytes, 4):
            a = src_local[i + 3]
            da = 255 - a
            sa = a
            # R channel
            v = (src_local[i] * sa + dst_local[i] * da + 127) // 255
            h ^= v
            h = (h * prime) & mask
            # G channel
            v = (src_local[i + 1] * sa + dst_local[i + 1] * da + 127) // 255
            h ^= v
            h = (h * prime) & mask
            # B channel
            v = (src_local[i + 2] * sa + dst_local[i + 2] * da + 127) // 255
            h ^= v
            h = (h * prime) & mask
            # A channel (constant 255)
            h ^= out_alpha
            h = (h * prime) & mask
    return h
