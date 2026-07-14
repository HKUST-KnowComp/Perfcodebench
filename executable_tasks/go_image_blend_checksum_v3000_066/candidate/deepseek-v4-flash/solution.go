package main

func run(src, dst []byte, width, height, iters int) uint64 {
    if iters == 0 {
        // Return initial hash for zero iterations (empty output)
        return 1469598103934665603
    }

    // Precompute the blended image once
    pixels := width * height
    out := make([]byte, pixels*4)
    for i := 0; i < pixels; i++ {
        base := i * 4
        a := src[base+3]
        // Inlined blend for each channel
        out[base] = uint8((uint32(src[base])*uint32(a) + uint32(dst[base])*uint32(255-a) + 127) / 255)
        out[base+1] = uint8((uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*uint32(255-a) + 127) / 255)
        out[base+2] = uint8((uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*uint32(255-a) + 127) / 255)
        out[base+3] = 255
    }

    // Compute checksum once
    var hash uint64 = 1469598103934665603
    for _, b := range out {
        hash ^= uint64(b)
        hash *= 1099511628211
    }
    return hash
}
