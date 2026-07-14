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
    h = 0
    for _ in range(iters):
        out = bytes(
            b
            for i in range(pixels)
            for b in (
                blend_byte(src[i * 4], dst[i * 4], src[i * 4 + 3]),
                blend_byte(src[i * 4 + 1], dst[i * 4 + 1], src[i * 4 + 3]),
                blend_byte(src[i * 4 + 2], dst[i * 4 + 2], src[i * 4 + 3]),
                255,
            )
        )
        h = checksum_bytes(out)
    return h