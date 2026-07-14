package main

func run(src, dst []byte, width, height, iters int) uint64 {
    if iters == 0 {
        return 0
    }
    pixels := width * height
    hash := uint64(1469598103934665603)
    for i := 0; i < pixels; i++ {
        base := i * 4
        a := uint32(src[base+3])
        inv := 255 - a
        // red
        hr := (uint32(src[base])*a + uint32(dst[base])*inv + 127) / 255
        hash ^= uint64(hr)
        hash *= 1099511628211
        // green
        hg := (uint32(src[base+1])*a + uint32(dst[base+1])*inv + 127) / 255
        hash ^= uint64(hg)
        hash *= 1099511628211
        // blue
        hb := (uint32(src[base+2])*a + uint32(dst[base+2])*inv + 127) / 255
        hash ^= uint64(hb)
        hash *= 1099511628211
        // alpha (always 255)
        hash ^= uint64(255)
        hash *= 1099511628211
    }
    return hash
}
