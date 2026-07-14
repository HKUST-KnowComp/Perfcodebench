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
	const (
		initHash = 1469598103934665603
		prime    = 1099511628211
	)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := initHash
		for i := range ids {
			// id (4 bytes little-endian)
			h ^= uint64(byte(ids[i]))
			h *= prime
			h ^= uint64(byte(ids[i] >> 8))
			h *= prime
			h ^= uint64(byte(ids[i] >> 16))
			h *= prime
			h ^= uint64(byte(ids[i] >> 24))
			h *= prime
			// ts (4 bytes little-endian)
			h ^= uint64(byte(ts[i]))
			h *= prime
			h ^= uint64(byte(ts[i] >> 8))
			h *= prime
			h ^= uint64(byte(ts[i] >> 16))
			h *= prime
			h ^= uint64(byte(ts[i] >> 24))
			h *= prime
			// levels (2 bytes little-endian)
			h ^= uint64(byte(levels[i]))
			h *= prime
			h ^= uint64(byte(levels[i] >> 8))
			h *= prime
			// lens (2 bytes little-endian)
			h ^= uint64(byte(lens[i]))
			h *= prime
			h ^= uint64(byte(lens[i] >> 8))
			h *= prime
		}
		hash = h
	}
	return hash
}
