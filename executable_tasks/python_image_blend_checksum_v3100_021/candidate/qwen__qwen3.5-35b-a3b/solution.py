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
        out = bytearray(
            [
                ((src[i*4 + c] * a) + (dst[i*4 + c] * (255 - a)) + 127) // 255
                for i in range(pixels)
                for c in range(3)
                for a in [src[i*4 + 3]]
            ] + [255] * pixels
        )
        h = checksum_bytes(out)
    return h