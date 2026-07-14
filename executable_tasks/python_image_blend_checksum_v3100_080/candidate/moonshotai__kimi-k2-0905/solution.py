import array

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
    out = array.array('B', [0]) * (pixels * 4)
    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)
    h = 0
    for _ in range(iters):
        o = 0
        for i in range(pixels):
            b = i * 4
            a = src_mv[b + 3]
            out_mv[o]   = blend_byte(src_mv[b],   dst_mv[b],   a)
            out_mv[o+1] = blend_byte(src_mv[b+1], dst_mv[b+1], a)
            out_mv[o+2] = blend_byte(src_mv[b+2], dst_mv[b+2], a)
            out_mv[o+3] = 255
            o += 4
        h = checksum_bytes(out_mv.tobytes())
    return h