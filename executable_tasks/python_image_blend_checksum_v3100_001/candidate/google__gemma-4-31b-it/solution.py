def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h = ((h ^ b) * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Pre-calculate constants
    # The blend formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # can be rewritten as: (s*a + d*255 - d*a + 127) // 255
    # = (a*(s-d) + 255*d + 127) // 255
    
    # To optimize, we use a local reference to the checksum function
    _checksum = checksum_bytes
    
    # We use a pre-allocated bytearray to avoid repeated allocations
    out = bytearray(pixels * 4)
    
    # Cache references to avoid global lookups
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    
    h = 0
    for _ in range(iters):
        # Using a list comprehension or map is often faster than .extend in a loop
        # However, for raw byte manipulation, we can use a generator
        # To maximize speed, we flatten the loop and avoid function calls
        
        res = []
        for i in range(0, pixels * 4, 4):
            a = src_get(i + 3)
            inv_a = 255 - a
            
            # Blend R, G, B
            res.append((src_get(i) * a + dst_get(i) * inv_a + 127) // 255)
            res.append((src_get(i+1) * a + dst_get(i+1) * inv_a + 127) // 255)
            res.append((src_get(i+2) * a + dst_get(i+2) * inv_a + 127) // 255)
            res.append(255)
            
        h = _checksum(bytes(res))
        
    return h

# Note: The above is a refined version. For the absolute fastest Python execution 
# without numpy, we use a bytearray and slice assignment or a list of integers.
# Let's provide the most optimized version using a list and bytes().

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    h = 0
    _checksum = checksum_bytes
    
    # Pre-calculate indices to avoid multiplication in loop
    indices = [i for i in range(0, pixels * 4, 4)]
    
    for _ in range(iters):
        out_list = [0] * (pixels * 4)
        for i in indices:
            a = src[i + 3]
            inv_a = 255 - a
            out_list[i] = (src[i] * a + dst[i] * inv_a + 127) // 255
            out_list[i+1] = (src[i+1] * a + dst[i+1] * inv_a + 127) // 255
            out_list[i+2] = (src[i+2] * a + dst[i+2] * inv_a + 127) // 255
            out_list[i+3] = 255
        h = _checksum(bytes(out_list))
    return h