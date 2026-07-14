import array

def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255

def checksum_bytes(data: bytes) -> int:
    h = 14695981039346656037
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    out_view = array.array('B', out)
    h = 0
    for _ in range(iters):
        src_view = memoryview(src)
        dst_view = memoryview(dst)
        o = 0
        for i in range(pixels):
            base = i * 4
            a = src_view[base + 3]
            out_view[o]   = blend_byte(src_view[base],     dst_view[base],     a)
            out_view[o+1] = blend_byte(src_view[base + 1], dst_view[base + 1], a)
            out_view[o+2] = blend_byte(src_view[base + 2], dst_view[base + 2], a)
            out_view[o+3] = 255
            o += 4
        h = checksum_bytes(out)
    return h