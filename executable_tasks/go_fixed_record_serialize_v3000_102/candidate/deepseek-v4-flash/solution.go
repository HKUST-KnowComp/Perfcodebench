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
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		n := len(ids)
		for i := 0; i < n; i++ {
			// id (4 bytes, little-endian)
			v := ids[i]
			hash ^= uint64(v & 0xff)
			hash *= 1099511628211
			hash ^= uint64((v >> 8) & 0xff)
			hash *= 1099511628211
			hash ^= uint64((v >> 16) & 0xff)
			hash *= 1099511628211
			hash ^= uint64(v >> 24)
			hash *= 1099511628211

			// ts (4 bytes, little-endian)
			v = ts[i]
			hash ^= uint64(v & 0xff)
			hash *= 1099511628211
			hash ^= uint64((v >> 8) & 0xff)
			hash *= 1099511628211
			hash ^= uint64((v >> 16) & 0xff)
			hash *= 1099511628211
			hash ^= uint64(v >> 24)
			hash *= 1099511628211

			// level (2 bytes, little-endian)
			w := levels[i]
			hash ^= uint64(w & 0xff)
			hash *= 1099511628211
			hash ^= uint64(w >> 8)
			hash *= 1099511628211

			// len (2 bytes, little-endian)
			w = lens[i]
			hash ^= uint64(w & 0xff)
			hash *= 1099511628211
			hash ^= uint64(w >> 8)
			hash *= 1099511628211
		}
	}
	return hash
}
