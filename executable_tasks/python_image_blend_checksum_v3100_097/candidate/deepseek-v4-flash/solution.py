def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    h0 = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    pixels = width * height
    stride = pixels * 4
    s = src
    d = dst
    for _ in range(iters):
        h = h0
        for idx in range(0, stride, 4):
            a = s[idx + 3]
            inv_a = 255 - a
            r = ((s[idx] * a) + (d[idx] * inv_a) + 127) // 255
            g = ((s[idx+1] * a) + (d[idx+1] * inv_a) + 127) // 255
            b = ((s[idx+2] * a) + (d[idx+2] * inv_a) + 127) // 255
            h ^= r
            h = (h * prime) & mask
            h ^= g
            h = (h * prime) & mask
            h ^= b
            h = (h * prime) & mask
            h ^= 255
            h = (h * prime) & mask
    return h