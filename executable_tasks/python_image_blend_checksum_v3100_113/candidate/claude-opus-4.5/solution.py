def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out_size = pixels * 4
    
    # Pre-compute blend lookup table: blend_lut[a][s][d] = blended value
    # Too large, instead compute blend_lut[a] = (a, 255-a) for quick access
    # Actually, let's precompute all possible blend results for each alpha
    # blend_byte(s, d, a) = ((s * a) + (d * (255 - a)) + 127) // 255
    
    # Create lookup tables for blend computation
    # For each alpha value, precompute coefficients
    inv_255 = 1.0 / 255.0
    
    # Precompute blend table: blend_table[a][s*256+d] but that's 256*256*256 = 16M entries
    # Instead, use vectorized approach with precomputed alpha weights
    
    src_view = memoryview(src)
    dst_view = memoryview(dst)
    
    # Precompute FNV-1a constants
    FNV_PRIME = 1099511628211
    FNV_OFFSET = 1469598103934665603
    MASK64 = (1 << 64) - 1
    
    # Precompute powers of FNV_PRIME for 4-byte chunks
    # For checksum: h = ((((h ^ b0) * P ^ b1) * P ^ b2) * P ^ b3) * P
    # = h * P^4 ^ b0 * P^4 ^ b1 * P^3 ^ b2 * P^2 ^ b3 * P
    p1 = FNV_PRIME
    p2 = (p1 * FNV_PRIME) & MASK64
    p3 = (p2 * FNV_PRIME) & MASK64
    p4 = (p3 * FNV_PRIME) & MASK64
    
    # Precompute 255 * P^4 for the constant alpha=255 byte
    alpha_contrib = (255 * p1) & MASK64
    
    # Precompute blend results for all (s, d, a) combinations - too large
    # Instead, precompute for each alpha: coeff_s = a, coeff_d = 255 - a
    
    h = 0
    out = bytearray(out_size)
    
    for _ in range(iters):
        h = FNV_OFFSET
        idx = 0
        
        for i in range(pixels):
            base = i * 4
            a = src_view[base + 3]
            inv_a = 255 - a
            
            # Blend RGB channels
            s0, s1, s2 = src_view[base], src_view[base + 1], src_view[base + 2]
            d0, d1, d2 = dst_view[base], dst_view[base + 1], dst_view[base + 2]
            
            r = ((s0 * a) + (d0 * inv_a) + 127) // 255
            g = ((s1 * a) + (d1 * inv_a) + 127) // 255
            b = ((s2 * a) + (d2 * inv_a) + 127) // 255
            
            out[idx] = r
            out[idx + 1] = g
            out[idx + 2] = b
            out[idx + 3] = 255
            
            # Inline checksum for these 4 bytes
            # h = ((h ^ r) * P ^ g) * P ^ b) * P ^ 255) * P
            h = ((((((((h ^ r) * p1) & MASK64) ^ g) * p1) & MASK64) ^ b) * p1) & MASK64
            h = ((h ^ 255) * p1) & MASK64
            
            idx += 4
    
    return h
