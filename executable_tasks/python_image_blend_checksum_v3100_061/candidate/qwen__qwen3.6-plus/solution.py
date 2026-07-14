def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    length = width * height * 4
    h = 0
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = 0xFFFFFFFFFFFFFFFF
    for _ in range(iters):
        h = FNV_OFFSET
        for i in range(0, length, 4):
            a = src[i+3]
            h ^= ((src[i] - dst[i]) * a + 127) // 255 + dst[i]
            h = (h * FNV_PRIME) & MASK64
            h ^= ((src[i+1] - dst[i+1]) * a + 127) // 255 + dst[i+1]
            h = (h * FNV_PRIME) & MASK64
            h ^= ((src[i+2] - dst[i+2]) * a + 127) // 255 + dst[i+2]
            h = (h * FNV_PRIME) & MASK64
            h ^= 255
            h = (h * FNV_PRIME) & MASK64
    return h