def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h = ((h ^ b) * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    src_view = memoryview(src)
    dst_view = memoryview(dst)
    
    # Pre-calculate constants
    mask = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        out = bytearray(pixels * 4)
        out_view = memoryview(out)
        
        for i in range(pixels):
            base = i * 4
            a = src_view[base + 3]
            inv_a = 255 - a
            
            # Inline blend_byte: ((s * a) + (d * (255 - a)) + 127) // 255
            out_view[base] = ((src_view[base] * a) + (dst_view[base] * inv_a) + 127) // 255
            out_view[base + 1] = ((src_view[base + 1] * a) + (dst_view[base + 1] * inv_a) + 127) // 255
            out_view[base + 2] = ((src_view[base + 2] * a) + (dst_view[base + 2] * inv_a) + 127) // 255
            out_view[base + 3] = 255
            
        # Optimized checksum loop
        curr_h = 1469598103934665603
        for b in out:
            curr_h = ((curr_h ^ b) * 1099511628211) & mask
        h = curr_h
        
    return h