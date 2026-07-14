def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    for _ in range(iters):
        h = FNV_OFFSET
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            r = (src[base] * a + dst[base] * (255 - a) + 127) // 255
            g = (src[base + 1] * a + dst[base + 1] * (255 - a) + 127) // 255
            b = (src[base + 2] * a + dst[base + 2] * (255 - a) + 127) // 255
            h ^= r
            h = (h * FNV_PRIME) & MASK
            h ^= g
            h = (h * FNV_PRIME) & MASK
            h ^= b
            h = (h * FNV_PRIME) & MASK
            h ^= 255
            h = (h * FNV_PRIME) & MASK
    return h