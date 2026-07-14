package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const prime uint64 = 1099511628211
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := range ids {
			v := ids[i]
			hash ^= uint64(v & 0xFF)
			hash *= prime
			hash ^= uint64((v >> 8) & 0xFF)
			hash *= prime
			hash ^= uint64((v >> 16) & 0xFF)
			hash *= prime
			hash ^= uint64(v >> 24)
			hash *= prime

			v = ts[i]
			hash ^= uint64(v & 0xFF)
			hash *= prime
			hash ^= uint64((v >> 8) & 0xFF)
			hash *= prime
			hash ^= uint64((v >> 16) & 0xFF)
			hash *= prime
			hash ^= uint64(v >> 24)
			hash *= prime

			l := levels[i]
			hash ^= uint64(l & 0xFF)
			hash *= prime
			hash ^= uint64(l >> 8)
			hash *= prime

			ln := lens[i]
			hash ^= uint64(ln & 0xFF)
			hash *= prime
			hash ^= uint64(ln >> 8)
			hash *= prime
		}
	}
	return hash
}