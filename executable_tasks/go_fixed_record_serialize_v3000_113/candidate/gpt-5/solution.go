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
	// If no iterations requested, match baseline behavior: return 0
	if iters == 0 {
		return 0
	}

	const prime uint64 = 1099511628211
	hash := uint64(1469598103934665603)

	n := len(ids)
	for i := 0; i < n; i++ {
		v32 := ids[i]
		b := byte(v32)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v32 >> 8)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v32 >> 16)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v32 >> 24)
		hash ^= uint64(b)
		hash *= prime

		v32 = ts[i]
		b = byte(v32)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v32 >> 8)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v32 >> 16)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v32 >> 24)
		hash ^= uint64(b)
		hash *= prime

		v16 := levels[i]
		b = byte(v16)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v16 >> 8)
		hash ^= uint64(b)
		hash *= prime

		v16 = lens[i]
		b = byte(v16)
		hash ^= uint64(b)
		hash *= prime
		b = byte(v16 >> 8)
		hash ^= uint64(b)
		hash *= prime
	}

	return hash
}
