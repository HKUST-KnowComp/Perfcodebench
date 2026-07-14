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
	if iters == 0 {
		return 0
	}

	var hash uint64 = 1469598103934665603
	n := len(ids)

	for i := 0; i < n; i++ {
		// ids (uint32, 4 bytes, little-endian)
		b := byte(ids[i])
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(ids[i] >> 8)
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(ids[i] >> 16)
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(ids[i] >> 24)
		hash ^= uint64(b)
		hash *= 1099511628211

		// ts (uint32, 4 bytes, little-endian)
		b = byte(ts[i])
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(ts[i] >> 8)
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(ts[i] >> 16)
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(ts[i] >> 24)
		hash ^= uint64(b)
		hash *= 1099511628211

		// levels (uint16, 2 bytes, little-endian)
		b = byte(levels[i])
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(levels[i] >> 8)
		hash ^= uint64(b)
		hash *= 1099511628211

		// lens (uint16, 2 bytes, little-endian)
		b = byte(lens[i])
		hash ^= uint64(b)
		hash *= 1099511628211
		b = byte(lens[i] >> 8)
		hash ^= uint64(b)
		hash *= 1099511628211
	}

	return hash
}