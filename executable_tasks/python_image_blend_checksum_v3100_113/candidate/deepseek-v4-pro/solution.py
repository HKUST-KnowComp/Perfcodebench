def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters <= 0:
        return 0
    pixels = width * height
    out = bytearray(pixels * 4)
    src_b = src
    dst_b = dst
    out_b = out
    for i in range(pixels):
        base = i << 2
        a = src_b[base + 3]
        inv_a = 255 - a
        out_b[base] = (src_b[base] * a + dst_b[base] * inv_a + 127) // 255
        out_b[base + 1] = (src_b[base + 1] * a + dst_b[base + 1] * inv_a + 127) // 255
        out_b[base + 2] = (src_b[base + 2] * a + dst_b[base + 2] * inv_a + 127) // 255
        out_b[base + 3] = 255
    h = 1469598103934665603
    for b in out_b:
        h ^= b
        h = (h * 1099511628211) & 0xffffffffffffffff
    return h
