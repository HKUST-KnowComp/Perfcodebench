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
    # We can optimize the blend_byte logic: ((s * a) + (d * (255 - a)) + 127) // 255
    # Let's use a 2D table for each alpha: table[alpha][s, d]
    # Still too large. Let's use the fact that we can precompute for each alpha
    # the result of blend(s, d, a) if we iterate carefully.
    
    # To maximize speed in pure Python, we avoid function calls in the inner loop.
    # We use local variables and a pre-allocated buffer.
    
    h = 0
    for _ in range(iters):
        # Local references for speed
        _src = src
        _dst = dst
        _out = out
        
        for i in range(pixels):
            base = i * 4
            a = _src[base + 3]
            inv_a = 255 - a
            
            # Manual inlining of blend_byte
            # Red, Green, Blue
            _out[base] = ((_src[base] * a) + (_dst[base] * inv_a) + 127) // 255
            _out[base + 1] = ((_src[base + 1] * a) + (_dst[base + 1] * inv_a) + 127) // 255
            _out[base + 2] = ((_src[base + 2] * a) + (_dst[base + 2] * inv_a) + 127) // 255
            _out[base + 3] = 255
            
        h = checksum_bytes(_out)
    return h