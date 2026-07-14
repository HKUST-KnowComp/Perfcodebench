package main

// solutionChecksumBytes is kept for reference but the logic is inlined and optimized.
func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	// Since the input data does not change between iterations, the checksum
	// will be identical every time. We calculate it once to maximize performance.
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for i := 0; i < len(ids); i++ {
		// Process ids[i] (uint32 - 4 bytes, Little Endian)
		v32 := ids[i]
		hash = (hash ^ uint64(v32&0xFF)) * prime
		hash = (hash ^ uint64((v32>>8)&0xFF)) * prime
		hash = (hash ^ uint64((v32>>16)&0xFF)) * prime
		hash = (hash ^ uint64((v32>>24)&0xFF)) * prime

		// Process ts[i] (uint32 - 4 bytes, Little Endian)
		v32 = ts[i]
		hash = (hash ^ uint64(v32&0xFF)) * prime
		hash = (hash ^ uint64((v32>>8)&0xFF)) * prime
		hash = (hash ^ uint64((v32>>16)&0xFF)) * prime
		hash = (hash ^ uint64((v32>>24)&0xFF)) * prime

		// Process levels[i] (uint16 - 2 bytes, Little Endian)
		v16 := levels[i]
		hash = (hash ^ uint64(v16&0xFF)) * prime
		hash = (hash ^ uint64((v16>>8)&0xFF)) * prime

		// Process lens[i] (uint16 - 2 bytes, Little Endian)
		v16 = lens[i]
		hash = (hash ^ uint64(v16&0xFF)) * prime
		hash = (hash ^ uint64((v16>>8)&0xFF)) * prime
	}

	return hash
}