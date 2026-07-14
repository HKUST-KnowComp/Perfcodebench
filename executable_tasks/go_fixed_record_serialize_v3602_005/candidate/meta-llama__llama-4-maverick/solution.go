package main

func solutionChecksumBytes(data []byte) uint64 {
    var hash uint64 = 1469598103934665603
    for i := 0; i < len(data); i++ {
        hash ^= uint64(data[i])
        hash *= 1099511628211
    }
    return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
    size := len(ids) * 12
    out := make([]byte, size)
    var hash uint64
    for iter := 0; iter < iters; iter++ {
        for i := range ids {
            j := i * 12
            out[j] = byte(ids[i])
            out[j+1] = byte(ids[i]>>8)
            out[j+2] = byte(ids[i]>>16)
            out[j+3] = byte(ids[i]>>24)
            out[j+4] = byte(ts[i])
            out[j+5] = byte(ts[i]>>8)
            out[j+6] = byte(ts[i]>>16)
            out[j+7] = byte(ts[i]>>24)
            out[j+8] = byte(levels[i])
            out[j+9] = byte(levels[i]>>8)
            out[j+10] = byte(lens[i])
            out[j+11] = byte(lens[i]>>8)
        }
        hash = solutionChecksumBytes(out)
    }
    return hash
}
