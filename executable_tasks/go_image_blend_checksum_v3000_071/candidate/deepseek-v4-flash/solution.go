package main

func run(src, dst []byte, width, height, iters int) uint64 {
    pixels := width * height
    var hash uint64
    const fnvOffset = 1469598103934665603
    const fnvPrime = 1099511628211

    for iter := 0; iter < iters; iter++ {
        h := fnvOffset
        for i := 0; i < pixels; i++ {
            base := i * 4
            a := uint32(src[base+3])

            // Blend R and update hash
            val := (uint32(src[base])*a + uint32(dst[base])*(255-a) + 127) / 255
            h ^= uint64(val)
            h *= fnvPrime

            // Blend G
            val = (uint32(src[base+1])*a + uint32(dst[base+1])*(255-a) + 127) / 255
            h ^= uint64(val)
            h *= fnvPrime

            // Blend B
            val = (uint32(src[base+2])*a + uint32(dst[base+2])*(255-a) + 127) / 255
            h ^= uint64(val)
            h *= fnvPrime

            // Alpha is always 255
            h ^= 255
            h *= fnvPrime
        }
        hash = h
    }
    return hash
}
