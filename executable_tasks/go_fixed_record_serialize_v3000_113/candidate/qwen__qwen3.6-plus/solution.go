package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < n; i++ {
			id := ids[i]
			hash ^= uint64(id)
			hash *= 1099511628211
			hash ^= uint64(id >> 8)
			hash *= 1099511628211
			hash ^= uint64(id >> 16)
			hash *= 1099511628211
			hash ^= uint64(id >> 24)
			hash *= 1099511628211

			t := ts[i]
			hash ^= uint64(t)
			hash *= 1099511628211
			hash ^= uint64(t >> 8)
			hash *= 1099511628211
			hash ^= uint64(t >> 16)
			hash *= 1099511628211
			hash ^= uint64(t >> 24)
			hash *= 1099511628211

			l := levels[i]
			hash ^= uint64(l)
			hash *= 1099511628211
			hash ^= uint64(l >> 8)
			hash *= 1099511628211

			ln := lens[i]
			hash ^= uint64(ln)
			hash *= 1099511628211
			hash ^= uint64(ln >> 8)
			hash *= 1099511628211
		}
	}
	return hash
}
