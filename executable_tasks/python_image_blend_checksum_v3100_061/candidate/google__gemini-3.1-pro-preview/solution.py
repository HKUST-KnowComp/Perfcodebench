def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    # Pre-fill alpha channel as it is constant 255
    out[3::4] = b'\xff' * pixels

    # Use memoryview for efficient slicing without copying
    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    # Extract alpha values once per iteration
    # Formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # Simplified: d + (a * (s - d) + 127) // 255

    h = 0
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF

    for _ in range(iters):
        a_vals = src_mv[3::4]
        
        # Process R, G, B channels separately using list comprehensions
        # This is significantly faster than a per-pixel Python loop
        for c in range(3):
            s_vals = src_mv[c::4]
            d_vals = dst_mv[c::4]
            
            # The list comprehension is executed in optimized C code internally
            out_mv[c::4] = bytes(
                (d + (a * (s - d) + 127) // 255)
                for s, d, a in zip(s_vals, d_vals, a_vals)
            )

        # Optimized FNV-1a checksum loop
        h = FNV_OFFSET
        for b in out:
            h = ((h ^ b) * FNV_PRIME) & MASK
            
    return h