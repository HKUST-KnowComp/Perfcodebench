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
		hashMul  = 1099511628211
	)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = initHash
		for i := range ids {
			// id (4 bytes, little-endian)
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= hashMul
			hash ^= uint64(byte(v >> 8))
			hash *= hashMul
			hash ^= uint64(byte(v >> 16))
			hash *= hashMul
			hash ^= uint64(byte(v >> 24))
			hash *= hashMul

			// ts (4 bytes, little-endian)
			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= hashMul
			hash ^= uint64(byte(v >> 8))
			hash *= hashMul
			hash ^= uint64(byte(v >> 16))
			hash *= hashMul
			hash ^= uint64(byte(v >> 24))
			hash *= hashMul

			// level (2 bytes, little-endian)
			w := levels[i]
			hash ^= uint64(byte(w))
			hash *= hashMul
			hash ^= uint64(byte(w >> 8))
			hash *= hashMul

			// len (2 bytes, little-endian)
			w = lens[i]
			hash ^= uint64(byte(w))
			hash *= hashMul
			hash ^= uint64(byte(w >> 8))
			hash *= hashMul
		}
	}
	return hash
}
