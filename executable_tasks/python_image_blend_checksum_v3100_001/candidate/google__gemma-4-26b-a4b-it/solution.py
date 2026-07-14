def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Pre-calculate the blending logic to avoid function call overhead
    # The formula is: ((s * a) + (d * (255 - a)) + 127) // 255
    
    # To optimize in pure Python, we use local variables and avoid .extend()
    # We use a pre-allocated bytearray to avoid repeated allocations
    out = bytearray(pixels * 4)
    out_view = memoryview(out)
    src_view = memoryview(src)
    dst_view = memoryview(dst)

    # Pre-calculate constants
    # Since we cannot use numpy, we optimize the loop structure
    
    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i << 2
            a = src_view[base + 3]
            
            # Optimization: if alpha is 255, result is src; if 0, result is dst
            # But we must follow the exact formula provided
            inv_a = 255 - a
            
            out_view[base]     = ((src_view[base] * a) + (dst_view[base] * inv_a) + 127) // 255
            out_view[base + 1] = ((src_view[base + 1] * a) + (dst_view[base + 1] * inv_a) + 127) // 255
            out_view[base + 2] = ((src_view[base + 2] * a) + (dst_view[base + 2] * inv_a) + 127) // 255
            out_view[base + 3] = 255
            
        # Checksum calculation
        h = 1469598103934665603
        for b in out:
            h ^= b
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h