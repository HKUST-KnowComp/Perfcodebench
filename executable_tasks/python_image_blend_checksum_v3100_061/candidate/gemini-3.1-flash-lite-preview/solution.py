def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            ia = 255 - a
            out_mv[base] = ((src[base] * a) + (dst[base] * ia) + 127) // 255
            out_mv[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * ia) + 127) // 255
            out_mv[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * ia) + 127) // 255
            out_mv[base + 3] = 255
        h = checksum_bytes(out)
    return h