package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := range ids {
			// Process ids[i] (4 bytes)
			v := ids[i]
			hash ^= uint64(v); hash *= fnvPrime
			v >>= 8
			hash ^= uint64(v); hash *= fnvPrime
			v >>= 8
			hash ^= uint64(v); hash *= fnvPrime
			v >>= 8
			hash ^= uint64(v); hash *= fnvPrime

			// Process ts[i] (4 bytes)
			v = ts[i]
			hash ^= uint64(v); hash *= fnvPrime
			v >>= 8
			hash ^= uint64(v); hash *= fnvPrime
			v >>= 8
			hash ^= uint64(v); hash *= fnvPrime
			v >>= 8
			hash ^= uint64(v); hash *= fnvPrime

			// Process levels[i] (2 bytes)
			v2 := uint32(levels[i])
			hash ^= uint64(v2); hash *= fnvPrime
			v2 >>= 8
			hash ^= uint64(v2); hash *= fnvPrime

			// Process lens[i] (2 bytes)
			v2 = uint32(lens[i])
			hash ^= uint64(v2); hash *= fnvPrime
			v2 >>= 8
			hash ^= uint64(v2); hash *= fnvPrime
		}
	}
	return hash
}
