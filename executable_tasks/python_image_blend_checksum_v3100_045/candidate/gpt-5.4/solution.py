def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels4 = width * height * 4
    mask = (1 << 64) - 1
    prime = 1099511628211

    # Precompute exact rounded division by 255 for all possible
    # x = s*a + d*(255-a), where x in [0, 65025].
    div255 = [0] * 65026
    for x in range(65026):
        div255[x] = (x + 127) // 255

    h = 0
    for _ in range(iters):
        out = bytearray(pixels4)
        o = out
        s = src
        d = dst

        j = 0
        for base in range(0, pixels4, 4):
            a = s[base + 3]
            ia = 255 - a
            o[j] = div255[s[base] * a + d[base] * ia]
            o[j + 1] = div255[s[base + 1] * a + d[base + 1] * ia]
            o[j + 2] = div255[s[base + 2] * a + d[base + 2] * ia]
            o[j + 3] = 255
            j += 4

        hh = 1469598103934665603
        for b in o:
            hh ^= b
            hh = (hh * prime) & mask
        h = hh

    return h
