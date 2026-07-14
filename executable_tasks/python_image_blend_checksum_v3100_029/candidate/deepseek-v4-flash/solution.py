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
    out = bytearray(pixels * 4)
    src_local = src
    dst_local = dst
    out_local = out
    for i in range(pixels):
        base = i * 4
        a = src_local[base + 3]
        inv_a = 255 - a
        r = (src_local[base] * a + dst_local[base] * inv_a + 127) // 255
        g = (src_local[base + 1] * a + dst_local[base + 1] * inv_a + 127) // 255
        b = (src_local[base + 2] * a + dst_local[base + 2] * inv_a + 127) // 255
        out_local[base] = r
        out_local[base + 1] = g
        out_local[base + 2] = b
        out_local[base + 3] = 255
    h = 1469598103934665603
    for b in out_local:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h