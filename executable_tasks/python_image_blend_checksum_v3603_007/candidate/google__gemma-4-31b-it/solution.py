def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h ^= b
        h = (h * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Precompute blend table: table[alpha][src_val][dst_val]
    # However, a 256*256*256 table is too large. 
    # Let's use a simpler approach: precompute the blend for each alpha
    # since alpha is the same for R, G, B.
    # To keep it fast in Python, we can use a 2D table for (src, dst) given alpha
    # but alpha changes per pixel. 
    # Instead, we can optimize the blend_byte logic.
    
    # Optimized blend: ((s * a) + (d * (255 - a)) + 127) // 255
    # We can precompute a table for each alpha: table[alpha][s][d]
    # But that's 256^3. Let's use a 1D table for the common case or just optimize the loop.
    
    # Actually, the most efficient way in pure Python without numpy is to avoid 
    # function calls and use local variables.
    
    h = 0
    for _ in range(iters):
        # We use a local reference to the bytearray for speed
        # and avoid the overhead of .extend()
        idx = 0
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            # Manual inlining of blend_byte
            # R
            s0 = src[base]
            d0 = dst[base]
            out[idx] = ((s0 * a) + (d0 * inv_a) + 127) // 255
            # G
            s1 = src[base + 1]
            d1 = dst[base + 1]
            out[idx + 1] = ((s1 * a) + (d1 * inv_a) + 127) // 255
            # B
            s2 = src[base + 2]
            d2 = dst[base + 2]
            out[idx + 2] = ((s2 * a) + (d2 * inv_a) + 127) // 255
            # A
            out[idx + 3] = 255
            idx += 4
            
        h = checksum_bytes(out)
    return h