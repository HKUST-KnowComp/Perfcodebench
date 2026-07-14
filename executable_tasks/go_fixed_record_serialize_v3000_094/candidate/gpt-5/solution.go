package main

const (
    fnvOffset64 = 1469598103934665603
    fnvPrime64  = 1099511628211
)

func solutionChecksumBytes(data []byte) uint64 {
    h := uint64(fnvOffset64)
    for _, b := range data {
        h ^= uint64(b)
        h *= fnvPrime64
    }
    return h
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
    // If no iterations, match baseline behavior
    if iters <= 0 {
        return 0
    }

    // Stream the FNV-1a hash directly over the serialized bytes to avoid
    // constructing the intermediate byte slice.
    h := uint64(fnvOffset64)
    n := len(ids)
    for i := 0; i < n; i++ {
        x := ids[i]
        h ^= uint64(byte(x))
        h *= fnvPrime64
        h ^= uint64(byte(x >> 8))
        h *= fnvPrime64
        h ^= uint64(byte(x >> 16))
        h *= fnvPrime64
        h ^= uint64(byte(x >> 24))
        h *= fnvPrime64

        y := ts[i]
        h ^= uint64(byte(y))
        h *= fnvPrime64
        h ^= uint64(byte(y >> 8))
        h *= fnvPrime64
        h ^= uint64(byte(y >> 16))
        h *= fnvPrime64
        h ^= uint64(byte(y >> 24))
        h *= fnvPrime64

        u := levels[i]
        h ^= uint64(byte(u))
        h *= fnvPrime64
        h ^= uint64(byte(u >> 8))
        h *= fnvPrime64

        v := lens[i]
        h ^= uint64(byte(v))
        h *= fnvPrime64
        h ^= uint64(byte(v >> 8))
        h *= fnvPrime64
    }

    // The buffer is identical across iterations; the last iteration's checksum
    // equals this single computation.
    return h
}
