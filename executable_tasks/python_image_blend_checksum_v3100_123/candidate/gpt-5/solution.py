def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Compute FNV-1a checksum of the blended output for the last iteration.
    pixels = width * height
    size = pixels * 4

    sb = src  # local references for speed
    db = dst

    prime = 1099511628211
    mask = (1 << 64) - 1
    const255 = 255

    h = 0
    for _ in range(iters):
        # Start a fresh checksum for each iteration (baseline behavior keeps only last iteration)
        h_iter = 1469598103934665603
        i = 0
        # Process RGBA per pixel
        while i < size:
            a = sb[i + 3]
            inv = const255 - a

            # R channel
            c = (sb[i] * a + db[i] * inv + 127) // 255
            h_iter ^= c
            h_iter = (h_iter * prime) & mask

            # G channel
            j = i + 1
            c = (sb[j] * a + db[j] * inv + 127) // 255
            h_iter ^= c
            h_iter = (h_iter * prime) & mask

            # B channel
            j = i + 2
            c = (sb[j] * a + db[j] * inv + 127) // 255
            h_iter ^= c
            h_iter = (h_iter * prime) & mask

            # A channel is always 255 in the output
            h_iter ^= const255
            h_iter = (h_iter * prime) & mask

            i += 4

        h = h_iter

    return h
