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
    out = bytearray(pixels * 4)          # pre-allocate once
    src_mv = memoryview(src).cast('B')
    dst_mv = memoryview(dst).cast('B')
    out_mv = memoryview(out).cast('B')

    h = 0
    for _ in range(iters):
        # blend and fill out_mv in one native loop
        for i in range(pixels):
            base = i * 4
            a = src_mv[base + 3]
            out_mv[base]     = blend_byte(src_mv[base],     dst_mv[base],     a)
            out_mv[base + 1] = blend_byte(src_mv[base + 1], dst_mv[base + 1], a)
            out_mv[base + 2] = blend_byte(src_mv[base + 2], dst_mv[base + 2], a)
            out_mv[base + 3] = 255
        # compute checksum directly on the buffer
        h = checksum_bytes(out_mv)
    return h