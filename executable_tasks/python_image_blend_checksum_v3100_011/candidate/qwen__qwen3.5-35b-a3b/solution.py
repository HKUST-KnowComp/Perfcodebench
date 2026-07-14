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
    h = 0
    for _ in range(iters):
        out_list = []
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            r = (src[base] * a + dst[base] * inv_a + 127) // 255
            g = (src[base + 1] * a + dst[base + 1] * inv_a + 127) // 255
            b = (src[base + 2] * a + dst[base + 2] * inv_a + 127) // 255
            out_list.extend([r, g, b, 255])
        out = bytearray(out_list)
        h = checksum_bytes(out)
    return h