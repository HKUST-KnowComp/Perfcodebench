def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0
    n = len(src)
    h = 1469598103934665603
    P = 1099511628211
    M = 0xFFFFFFFFFFFFFFFF
    for i in range(0, n, 4):
        a = src[i+3]
        ia = 255 - a
        v = (src[i] * a + dst[i] * ia + 127) // 255
        h ^= v; h = (h * P) & M
        v = (src[i+1] * a + dst[i+1] * ia + 127) // 255
        h ^= v; h = (h * P) & M
        v = (src[i+2] * a + dst[i+2] * ia + 127) // 255
        h ^= v; h = (h * P) & M
        h ^= 255; h = (h * P) & M
    return h