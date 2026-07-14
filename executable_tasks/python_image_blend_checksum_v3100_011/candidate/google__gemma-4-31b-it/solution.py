def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Precompute blend tables for all possible alpha values (0-255)
    # blend_byte(s, d, a) = ((s * a) + (d * (255 - a)) + 127) // 255
    # We can't precompute for all s, d, a (256^3), but we can precompute for a
    # However, a simpler optimization is to avoid the function call and use a local variable
    
    # To truly optimize in pure Python, we avoid the function call and use local variables
    # and pre-calculate the constants.
    
    # Pre-calculate the blend for every possible alpha value to avoid division
    # Since we can't precompute s and d, we optimize the loop structure.
    
    # Local references for speed
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    out_set = out.__setitem__

    # Precompute the checksum constants
    H_INIT = 1469598103934665603
    H_MULT = 1099511628211
    MASK = (1 << 64) - 1

    h = 0
    for _ in range(iters):
        # Blending loop
        for i in range(pixels * 4):
            # Process in chunks of 4 to handle RGBA
            if i % 4 == 3:
                # This is the alpha channel
                out[i] = 255
                continue
            
            # This is a bit slow. Let's use a more efficient loop.
            pass

        # Re-implementing the loop for better performance
        idx = 0
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            # Red
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            # Green
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            # Blue
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            # Alpha
            out[base + 3] = 255

        # Checksum loop
        curr_h = H_INIT
        for b in out:
            curr_h = ((curr_h ^ b) * H_MULT) & MASK
        h = curr_h
        
    return h

# The above is still slow. Let's use a more aggressive approach for Python.
# Since we are limited to standard libraries, we use a precomputed table for the blend
# if possible, or just optimize the loop as much as possible.

# Final optimized version
def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Precompute the blend for all possible alpha values for a specific s, d is too much
    # But we can optimize the inner loop
    
    H_INIT = 1469598103934665603
    H_MULT = 1099511628211
    MASK = (1 << 64) - 1

    final_h = 0
    for _ in range(iters):
        # Optimized blending
        for i in range(0, pixels * 4, 4):
            a = src[i + 3]
            inv_a = 255 - a
            out[i] = ((src[i] * a) + (dst[i] * inv_a) + 127) // 255
            out[i+1] = ((src[i+1] * a) + (dst[i+1] * inv_a) + 127) // 255
            out[i+2] = ((src[i+2] * a) + (dst[i+2] * inv_a) + 127) // 255
            out[i+3] = 255

        # Optimized checksum
        h = H_INIT
        for b in out:
            h = ((h ^ b) * H_MULT) & MASK
        final_h = h
        
    return final_h