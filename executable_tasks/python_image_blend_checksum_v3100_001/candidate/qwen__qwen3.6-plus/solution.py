def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    n = width * height * 4
    M = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    INIT = 1469598103934665603
    h = 0
    for _ in range(iters):
        h = INIT
        for i in range(0, n, 4):
            a = src[i+3]
            dr = dst[i]
            h = ((h ^ (((src[i] - dr) * a + 127) // 255 + dr)) * M) & MASK
            dg = dst[i+1]
            h = ((h ^ (((src[i+1] - dg) * a + 127) // 255 + dg)) * M) & MASK
            db = dst[i+2]
            h = ((h ^ (((src[i+2] - db) * a + 127) // 255 + db)) * M) & MASK
            h = ((h ^ 255) * M) & MASK
    return h