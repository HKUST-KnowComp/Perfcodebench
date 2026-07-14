def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h ^= b
        h = (h * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Precompute blend table: table[alpha][src_val][dst_val]
    # blend = ((s * a) + (d * (255 - a)) + 127) // 255
    # To optimize, we can use a 2D table for each alpha or a 3D table.
    # Given memory constraints and Python speed, a 2D table for each alpha is fast.
    # However, a simpler approach is to precompute the blend for all s, d, a.
    # But that's 256^3. Let's use a lookup for (s, d, a).
    # Actually, the most efficient way in pure Python without numpy is to avoid the function call.
    
    # Precompute the blend results for all possible s, d, a is too large.
    # But we can precompute for each alpha: table[a][s][d]
    # Even better: since we only have 256 possible alphas, we can precompute
    # the blend for each alpha and each possible byte value.
    
    # Let's use a more direct optimization: avoid function calls and use a local variable for the checksum.
    # Since we need to return the checksum of the LAST iteration:
    
    out = bytearray(pixels * 4)
    
    # Precompute blend table for all s, d, a is 256^3 = 16.7M entries. 
    # That's too big for a Python list. 
    # Let's precompute a table for each alpha: blend_table[alpha][s][d]
    # Still too big. Let's precompute blend_table[alpha][s] and blend_table[alpha][d].
    # blend = (s*a + d*(255-a) + 127) // 255
    
    # Optimization: The blend formula is linear. 
    # We can precompute the results for each alpha and each byte value.
    # But the formula depends on both s and d.
    
    # Let's optimize the inner loop as much as possible.
    
    h = 0
    for _ in range(iters):
        idx = 0
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            # Manual inlining of blend_byte
            s0, d0 = src[base], dst[base]
            out[idx] = ((s0 * a) + (d0 * inv_a) + 127) // 255
            
            s1, d1 = src[base + 1], dst[base + 1]
            out[idx + 1] = ((s1 * a) + (d1 * inv_a) + 127) // 255
            
            s2, d2 = src[base + 2], dst[base + 2]
            out[idx + 2] = ((s2 * a) + (d2 * inv_a) + 127) // 255
            
            out[idx + 3] = 255
            idx += 4
        h = checksum_bytes(out)
    return h