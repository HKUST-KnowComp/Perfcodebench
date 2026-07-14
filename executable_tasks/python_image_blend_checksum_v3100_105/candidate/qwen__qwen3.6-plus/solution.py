def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    r_src = src[0::4]
    r_dst = dst[0::4]
    g_src = src[1::4]
    g_dst = dst[1::4]
    b_src = src[2::4]
    b_dst = dst[2::4]
    a_src = src[3::4]

    out = bytearray(
        v
        for sr, dr, sg, dg, sb, db, a in zip(r_src, r_dst, g_src, g_dst, b_src, b_dst, a_src)
        for v in (
            ((sr * a) + (dr * (255 - a)) + 127) // 255,
            ((sg * a) + (dg * (255 - a)) + 127) // 255,
            ((sb * a) + (db * (255 - a)) + 127) // 255,
            255
        )
    )

    h = 1469598103934665603
    m = (1 << 64) - 1
    p = 1099511628211
    for b in out:
        h ^= b
        h = (h * p) & m
    return h