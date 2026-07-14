package main

func solutionChecksumBytes(data []byte) uint64 {
    var hash uint64 = 1469598103934665603
    for _, b := range data {
        hash ^= uint64(b)
        hash *= 1099511628211
    }
    return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
    out := make([]byte, 0, len(ids)*10*iters)
    var hash uint64
    for iter := 0; iter < iters; iter++ {
        out = out[:0]
        for i := range ids {
            out = append(out,
                byte(ids[i]), byte(ids[i]>>8), byte(ids[i]>>16), byte(ids[i]>>24),
                byte(ts[i]), byte(ts[i]>>8), byte(ts[i]>>16), byte(ts[i]>>24),
                byte(levels[i]), byte(levels[i]>>8),
                byte(lens[i]), byte(lens[i]>>8),
            )
        }
        hash = solutionChecksumBytes(out)
    }
    return hash
}
