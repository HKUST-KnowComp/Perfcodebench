def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h = ((h ^ b) * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Precompute blend table: table[alpha][src_val][dst_val]
    # To save memory and time, we can use a 2D table if we process alpha separately
    # or a 3D table. Given the constraints, a 2D table for each alpha is too large.
    # However, we can precompute the blend for all possible (s, d, a) if needed,
    # but that's 256^3. Instead, let's optimize the inner loop.
    
    # Precompute the blend formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # We can precompute a table for each alpha: blend_table[alpha][s][d]
    # But that's 256 * 256 * 256 = 16.7 million entries. 
    # Let's use a simpler approach: precompute the blend for each alpha and s, d
    # using a flat array or just optimize the loop with local variables.
    
    # Actually, the most efficient way in pure Python without numpy is to avoid 
    # function calls and use a precomputed table for the most common operations.
    
    # Precompute blend_table[alpha][s][d] is too big. 
    # Let's precompute blend_table[alpha][s] and blend_table[alpha][d] components?
    # No, the formula is (s*a + d*(255-a) + 127) // 255.
    
    # Let's use a precomputed table for the blend operation for all s, d for a specific a.
    # Since a is constant for a pixel, we can't easily. 
    # But we can precompute a table for (s, a) and (d, 255-a).
    
    # Let's try a more direct optimization: avoid function calls and use a pre-allocated bytearray.
    out = bytearray(pixels * 4)
    
    # Precompute the blend results for all possible s, d, a is too much.
    # But we can precompute the blend for all s, d for a fixed a? No.
    # Let's precompute the blend for all s, d for all a? No.
    
    # Let's use the fact that (s*a + d*(255-a) + 127) // 255 is the operation.
    # We can precompute a table for (val * alpha + 127) // 255? No.
    
    # Let's optimize the loop as much as possible.
    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            # Manual inlining of blend_byte
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255
        h = checksum_bytes(out)
    return h