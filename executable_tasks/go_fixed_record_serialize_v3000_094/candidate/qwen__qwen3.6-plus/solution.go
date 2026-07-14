package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	const prime = 1099511628211
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := range ids {
			v := ids[i]
			hash ^= uint64(v & 0xFF); hash *= prime
			hash ^= uint64((v >> 8) & 0xFF); hash *= prime
			hash ^= uint64((v >> 16) & 0xFF); hash *= prime
			hash ^= uint64(v >> 24); hash *= prime

			v = ts[i]
			hash ^= uint64(v & 0xFF); hash *= prime
			hash ^= uint64((v >> 8) & 0xFF); hash *= prime
			hash ^= uint64((v >> 16) & 0xFF); hash *= prime
			hash ^= uint64(v >> 24); hash *= prime

			v16 := uint32(levels[i])
			hash ^= uint64(v16 & 0xFF); hash *= prime
			hash ^= uint64(v16 >> 8); hash *= prime

			v16 = uint32(lens[i])
			hash ^= uint64(v16 & 0xFF); hash *= prime
			hash ^= uint64(v16 >> 8); hash *= prime
		}
	}
	return hash
}