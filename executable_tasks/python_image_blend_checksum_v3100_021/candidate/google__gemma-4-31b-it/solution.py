def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h = ((h ^ b) * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate constants to avoid repeated lookups
    # The blend formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # can be rewritten as: (s * a + d * 255 - d * a + 127) // 255
    
    # To optimize, we process the image in a way that minimizes Python overhead.
    # Since we must return the checksum of the final 'out' buffer,
    # we optimize the inner loop.
    
    h = 0
    for _ in range(iters):
        # Local references for speed
        s_ref = src
        d_ref = dst
        
        # We use a loop that avoids function calls and minimizes indexing overhead
        for i in range(0, pixels * 4, 4):
            a = s_ref[i + 3]
            inv_a = 255 - a
            
            # Red
            out[i] = ((s_ref[i] * a) + (d_ref[i] * inv_a) + 127) // 255
            # Green
            out[i + 1] = ((s_ref[i + 1] * a) + (d_ref[i + 1] * inv_a) + 127) // 255
            # Blue
            out[i + 2] = ((s_ref[i + 2] * a) + (d_ref[i + 2] * inv_a) + 127) // 255
            # Alpha
            out[i + 3] = 255
            
        h = checksum_bytes(out)
        
    return h