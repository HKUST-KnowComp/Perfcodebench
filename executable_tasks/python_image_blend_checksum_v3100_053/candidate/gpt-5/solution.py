def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Compute the FNV-1 hash over the blended RGBA output without materializing it.
    # Blend per channel: d + ((a * (s - d) + 127) // 255), alpha set to 255.
    pixels = width * height
    total = pixels * 4

    # FNV-1 64-bit constants
    offset_basis = 1469598103934665603
    fnv_prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    sarr = src  # local bindings for speed
    darr = dst

    h = 0
    for _ in range(iters):
        h = offset_basis
        # Iterate per pixel (4 bytes at a time)
        for i in range(0, total, 4):
            a = sarr[i + 3]

            # Red channel
            d0 = darr[i]
            v0 = d0 + ((a * (sarr[i] - d0) + 127) // 255)
            h = ((h ^ v0) * fnv_prime) & mask

            # Green channel
            d1 = darr[i + 1]
            v1 = d1 + ((a * (sarr[i + 1] - d1) + 127) // 255)
            h = ((h ^ v1) * fnv_prime) & mask

            # Blue channel
            d2 = darr[i + 2]
            v2 = d2 + ((a * (sarr[i + 2] - d2) + 127) // 255)
            h = ((h ^ v2) * fnv_prime) & mask

            # Alpha channel is constant 255
            h = ((h ^ 255) * fnv_prime) & mask

    return h
