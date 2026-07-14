def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Pre-calculate indices to avoid repeated multiplication
    indices = [i * 4 for i in range(pixels)]
    h = 0
    
    for _ in range(iters):
        out = bytearray(pixels * 4)
        for i in indices:
            a = src[i + 3]
            inv_a = 255 - a
            # Inline blending logic for performance
            out[i] = ((src[i] * a) + (dst[i] * inv_a) + 127) // 255
            out[i + 1] = ((src[i + 1] * a) + (dst[i + 1] * inv_a) + 127) // 255
            out[i + 2] = ((src[i + 2] * a) + (dst[i + 2] * inv_a) + 127) // 255
            out[i + 3] = 255
        h = checksum_bytes(out)
    return h