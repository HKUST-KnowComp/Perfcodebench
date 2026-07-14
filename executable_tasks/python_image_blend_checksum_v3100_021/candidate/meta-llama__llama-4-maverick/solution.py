```python
def blend_pixel(src: bytearray, dst: bytearray, out: bytearray, i: int) -> None:
    base = i * 4
    a = src[base + 3]
    a_inv = 255 - a
    out.extend(((
        (src[base] * a + dst[base] * a_inv + 127) >> 8,
        (src[base + 1] * a + dst[base + 1] * a_inv + 127) >> 8,
        (src[base + 2] * a + dst[base + 2] * a_inv + 127) >> 8,
        255,
    )))

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    out = bytearray()
    pixels = width * height
    for _ in range(iters):
        out.clear()
        for i in range(pixels):
            blend_pixel(src, dst, out, i)
        h = checksum_bytes(out)
    return h
```