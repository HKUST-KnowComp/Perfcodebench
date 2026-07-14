
def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0
    pixels = width * height
    h = 1469598103934665603
    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    # local aliases for speed
    prime = 1099511628211
    mask = (1 << 64) - 1
    for i in range(pixels):
        base = i * 4
        a = src_mv[base + 3]
        inv_a = 255 - a
        # blend each channel
        r = ((src_mv[base] * a) + (dst_mv[base] * inv_a) + 127) // 255
        g = ((src_mv[base + 1] * a) + (dst_mv[base + 1] * inv_a) + 127) // 255
        b = ((src_mv[base + 2] * a) + (dst_mv[base + 2] * inv_a) + 127) // 255
        # update hash with R, G, B, A=255
        h ^= r
        h = (h * prime) & mask
        h ^= g
        h = (h * prime) & mask
        h ^= b
        h = (h * prime) & mask
        h ^= 255
        h = (h * prime) & mask
    return h