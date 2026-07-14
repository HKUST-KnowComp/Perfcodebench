def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Precompute blend table: table[alpha][source_val][dest_val]
    # However, a 256^3 table is too large. 
    # We can use a 2D table for (s, d) given a fixed alpha, 
    # but alpha changes per pixel. 
    # Instead, we use the formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # To optimize, we can precompute the blend for all possible s, d for a given a?
    # No, let's use a faster way to calculate the blend.
    
    # Optimization: Precompute the blend for all possible s, d for each a is too much.
    # But we can precompute the blend for all s, d for a specific a if a is constant.
    # Since a varies, we can precompute a table for (s, a) and (d, a).
    # Actually, the most efficient way in pure Python without numpy is to minimize 
    # attribute lookups and function calls.

    # Precompute a 2D table for blend_byte(s, d, a) is too big (256^3).
    # Let's use a 1D table for the common case or just optimize the loop.
    
    # Let's use a precomputed table for the blend operation: 
    # blend(s, d, a) = (s*a + d*(255-a) + 127) // 255
    # We can precompute blend_table[a][s][d] if we had memory, but we don't.
    # However, we can precompute blend_table[a][s] = s * a and blend_table[a][d] = d * (255-a).
    
    s_a_table = [[(s * a) for s in range(256)] for a in range(256)]
    d_a_table = [[(d * (255 - a)) for d in range(256)] for a in range(256)]

    h = 0
    for _ in range(iters):
        idx = 0
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            
            sa = s_a_table[a]
            da = d_a_table[a]
            
            out[idx] = (sa[src[base]] + da[dst[base]] + 127) // 255
            out[idx + 1] = (sa[src[base + 1]] + da[dst[base + 1]] + 127) // 255
            out[idx + 2] = (sa[src[base + 2]] + da[dst[base + 2]] + 127) // 255
            out[idx + 3] = 255
            idx += 4
        h = checksum_bytes(out)
    return h