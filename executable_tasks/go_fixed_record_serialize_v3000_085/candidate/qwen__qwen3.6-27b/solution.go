package main

const (
	fnvOffset = 1469598103934665603
	fnvPrime  = 1099511628211
)

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = fnvOffset
	for _, b := range data {
		hash ^= uint64(b)
		hash *= fnvPrime
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := range ids {
			// ids[i] (4 bytes, little-endian)
			hash ^= uint64(ids[i]) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(ids[i]) >> 8) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(ids[i]) >> 16) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(ids[i]) >> 24) & 0xFF; hash *= fnvPrime
			// ts[i] (4 bytes, little-endian)
			hash ^= uint64(ts[i]) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(ts[i]) >> 8) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(ts[i]) >> 16) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(ts[i]) >> 24) & 0xFF; hash *= fnvPrime
			// levels[i] (2 bytes, little-endian)
			hash ^= uint64(levels[i]) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(levels[i]) >> 8) & 0xFF; hash *= fnvPrime
			// lens[i] (2 bytes, little-endian)
			hash ^= uint64(lens[i]) & 0xFF; hash *= fnvPrime
			hash ^= (uint64(lens[i]) >> 8) & 0xFF; hash *= fnvPrime
		}
	}
	return hash
}