def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    length = len(src)
    h = 0
    M = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    INIT = 1469598103934665603
    for _ in range(iters):
        h = INIT
        for i in range(0, length, 4):
            a = src[i+3]
            r = dst[i] + (a * (src[i] - dst[i]) + 127) // 255
            g = dst[i+1] + (a * (src[i+1] - dst[i+1]) + 127) // 255
            b = dst[i+2] + (a * (src[i+2] - dst[i+2]) + 127) // 255

            h = ((h ^ r) * M) & MASK
            h = ((h ^ g) * M) & MASK
            h = ((h ^ b) * M) & MASK
            h = ((h ^ 255) * M) & MASK
    return h