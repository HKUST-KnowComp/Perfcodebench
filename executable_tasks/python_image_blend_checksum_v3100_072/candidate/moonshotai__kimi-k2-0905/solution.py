import struct

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
    out = bytearray(pixels * 4)

    # Pre-build lookup tables for (s*a + d*(255-a) + 127)//255
    # We treat a as the outer dimension (256 entries) and (s,d) as 256×256 inner tables.
    blend_tbl = [[0]*65536 for _ in range(256)]
    for a in range(256):
        inv_a = 255 - a
        row = blend_tbl[a]
        for s in range(256):
            for d in range(256):
                row[(s << 8) | d] = ((s * a) + (d * inv_a) + 127) // 255

    # Build 8-bit -> 32-bit (LE) lookup for fast memcpy-style assignment
    dst32 = memoryview(out).cast('I')
    src16 = memoryview(src).cast('H')
    dst16 = memoryview(dst).cast('H')
    stride = width * height

    for _ in range(iters):
        for k in range(stride):
            s0, s1 = src16[k*2], src16[k*2+1]
            d0, d1 = dst16[k*2], dst16[k*2+1]
            a = (s1 >> 8) & 0xFF
            r = blend_tbl[a][((s0 >> 8) & 0xFF) << 8 | ((d0 >> 8) & 0xFF)]
            g = blend_tbl[a][((s0) & 0xFF) << 8 | ((d0) & 0xFF)]
            b = blend_tbl[a][((s1 >> 8) & 0xFF) << 8 | ((d1 >> 8) & 0xFF)]
            dst32[k] = 0xFF000000 | (b << 16) | (g << 8) | r

    return checksum_bytes(out)