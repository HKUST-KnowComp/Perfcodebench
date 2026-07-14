package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const prime = 1099511628211
	const offset = 1469598103934665603
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = offset
		for i := 0; i < n; i++ {
			// id (4 bytes, little-endian)
			v := uint64(ids[i])
			hash ^= v & 0xFF
			hash *= prime
			hash ^= (v >> 8) & 0xFF
			hash *= prime
			hash ^= (v >> 16) & 0xFF
			hash *= prime
			hash ^= (v >> 24) & 0xFF
			hash *= prime

			// ts (4 bytes, little-endian)
			v = uint64(ts[i])
			hash ^= v & 0xFF
			hash *= prime
			hash ^= (v >> 8) & 0xFF
			hash *= prime
			hash ^= (v >> 16) & 0xFF
			hash *= prime
			hash ^= (v >> 24) & 0xFF
			hash *= prime

			// level (2 bytes, little-endian)
			v = uint64(levels[i])
			hash ^= v & 0xFF
			hash *= prime
			hash ^= (v >> 8) & 0xFF
			hash *= prime

			// lens (2 bytes, little-endian)
			v = uint64(lens[i])
			hash ^= v & 0xFF
			hash *= prime
			hash ^= (v >> 8) & 0xFF
			hash *= prime
		}
	}
	return hash
}
