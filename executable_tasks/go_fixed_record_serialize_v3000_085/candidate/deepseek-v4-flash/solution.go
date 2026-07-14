package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
    n := len(ids)
    const fnvOffset uint64 = 1469598103934665603
    const fnvPrime uint64 = 1099511628211
    var hash uint64
    for iter := 0; iter < iters; iter++ {
        hash = fnvOffset
        for i := 0; i < n; i++ {
            v := ids[i]
            hash ^= uint64(v & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v >> 8) & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v >> 16) & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v >> 24) & 0xff)
            hash *= fnvPrime

            v = ts[i]
            hash ^= uint64(v & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v >> 8) & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v >> 16) & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v >> 24) & 0xff)
            hash *= fnvPrime

            v16 := uint32(levels[i])
            hash ^= uint64(v16 & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v16 >> 8) & 0xff)
            hash *= fnvPrime

            v16 = uint32(lens[i])
            hash ^= uint64(v16 & 0xff)
            hash *= fnvPrime
            hash ^= uint64((v16 >> 8) & 0xff)
            hash *= fnvPrime
        }
    }
    return hash
}
