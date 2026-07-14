package main

func run(src, dst []byte, width, height, iters int) uint64 {
    if iters == 0 {
        return 0
    }
    pixels := width * height
    var hash uint64 = 1469598103934665603
    for i := 0; i < pixels; i++ {
        base := i * 4
        a := src[base+3]
        invA := uint32(255 - a)
        // blend each channel and update hash
        b0 := uint8((uint32(src[base])*uint32(a) + uint32(dst[base])*invA + 127) / 255)
        hash ^= uint64(b0)
        hash *= 1099511628211
        b1 := uint8((uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*invA + 127) / 255)
        hash ^= uint64(b1)
        hash *= 1099511628211
        b2 := uint8((uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*invA + 127) / 255)
        hash ^= uint64(b2)
        hash *= 1099511628211
        hash ^= 255
        hash *= 1099511628211
    }
    return hash
}
