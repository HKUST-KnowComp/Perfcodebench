def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    FNV_INIT = 1469598103934665603
    
    mv_src = memoryview(src)
    mv_dst = memoryview(dst)
    
    sr = mv_src[0::4]
    sg = mv_src[1::4]
    sb = mv_src[2::4]
    sa = mv_src[3::4]
    dr = mv_dst[0::4]
    dg = mv_dst[1::4]
    db = mv_dst[2::4]
    
    h = 0
    for _ in range(iters):
        h = FNV_INIT
        for r, g, b, a, dr_val, dg_val, db_val in zip(sr, sg, sb, sa, dr, dg, db):
            inv_a = 255 - a
            h = ((h ^ ((r * a + dr_val * inv_a + 127) // 255)) * FNV_PRIME) & MASK
            h = ((h ^ ((g * a + dg_val * inv_a + 127) // 255)) * FNV_PRIME) & MASK
            h = ((h ^ ((b * a + db_val * inv_a + 127) // 255)) * FNV_PRIME) & MASK
            h = ((h ^ 255) * FNV_PRIME) & MASK
    return h