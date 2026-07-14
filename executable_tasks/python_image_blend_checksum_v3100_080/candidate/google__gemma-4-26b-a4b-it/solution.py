import numpy as np

def checksum_bytes(data: bytes) -> int:
    # FNV-1a 64-bit implementation
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized processing
    # Use uint32 for intermediate calculations to prevent overflow
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.uint32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.uint32)
    
    # Pre-extract alpha and components
    # src_arr[:, 0:3] are RGB, src_arr[:, 3] is A
    a = src_arr[:, 3:4]
    inv_a = 255 - a
    
    # The blending formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # We only blend the first 3 channels (RGB)
    blended_rgb = ((src_arr[:, :3] * a) + (dst_arr[:, :3] * inv_a) + 127) // 255
    
    # Construct the output array: blended RGB + constant 255 for Alpha
    # We create a full array of 255s and then overwrite the RGB parts
    out_arr = np.full((src_arr.shape[0], 4), 255, dtype=np.uint32)
    out_arr[:, :3] = blended_rgb
    
    # Convert back to uint8 for the checksum
    out_uint8 = out_arr.astype(np.uint8).tobytes()
    
    h = 0
    for _ in range(iters):
        h = checksum_bytes(out_uint8)
        
    return h