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
                val
                for i in range(pixels)
                for a, inv_a in [(src[i*4+3], 255 - src[i*4+3])]
                for val in (
                    ((src[i*4] * a) + (dst[i*4] * inv_a) + 127) // 255,
                    ((src[i*4+1] * a) + (dst[i*4+1] * inv_a) + 127) // 255,
                    ((src[i*4+2] * a) + (dst[i*4+2] * inv_a) + 127) // 255,
                    255,
                )
            ]
        )
        h = checksum_bytes(out)
    return h