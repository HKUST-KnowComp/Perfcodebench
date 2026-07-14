import sys

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
    # Pre-allocate output buffer once; reuse across iterations.
    out = bytearray(pixels * 4)
    src_view = memoryview(src)
    dst_view = memoryview(dst)
    out_view = memoryview(out)

    # Local bindings to avoid repeated lookups inside the loop.
    _blend = blend_byte
    _csum = checksum_bytes
    _range4 = range(4)

    h = 0
    for _ in range(iters):
        # Fast path: process 4-byte RGBA pixels in bulk.
        for i in range(pixels):
            base = i * 4
            s0, s1, s2, a = src_view[base:base + 4]
            d0, d1, d2 = dst_view[base:base + 3]
            out_base = base
            out_view[out_base]     = _blend(s0, d0, a)
            out_view[out_base + 1] = _blend(s1, d1, a)
            out_view[out_base + 2] = _blend(s2, d2, a)
            out_view[out_base + 3] = 255
        h = _csum(out)
    return h