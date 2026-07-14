package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const (
		offset64 uint64 = 1469598103934665603
		prime64  uint64 = 1099511628211
	)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = offset64
		for i := 0; i < len(ids); i++ {
			// Serialize and hash ids[i] (uint32, 4 bytes)
			v32 := ids[i]
			hash ^= uint64(v32 & 0xFF)
			hash *= prime64
			hash ^= uint64((v32 >> 8) & 0xFF)
			hash *= prime64
			hash ^= uint64((v32 >> 16) & 0xFF)
			hash *= prime64
			hash ^= uint64((v32 >> 24) & 0xFF)
			hash *= prime64

			// Serialize and hash ts[i] (uint32, 4 bytes)
			v32 = ts[i]
			hash ^= uint64(v32 & 0xFF)
			hash *= prime64
			hash ^= uint64((v32 >> 8) & 0xFF)
			hash *= prime64
			hash ^= uint64((v32 >> 16) & 0xFF)
			hash *= prime64
			hash ^= uint64((v32 >> 24) & 0xFF)
			hash *= prime64

			// Serialize and hash levels[i] (uint16, 2 bytes)
			v16 := levels[i]
			hash ^= uint64(v16 & 0xFF)
			hash *= prime64
			hash ^= uint64((v16 >> 8) & 0xFF)
			hash *= prime64

			// Serialize and hash lens[i] (uint16, 2 bytes)
			v16 = lens[i]
			hash ^= uint64(v16 & 0xFF)
			hash *= prime64
			hash ^= uint64((v16 >> 8) & 0xFF)
			hash *= prime64
		}
	}
	return hash
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}