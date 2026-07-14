package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := range ids {
			v := ids[i]
			b0 := byte(v)
			hash ^= uint64(b0)
			hash *= 1099511628211
			b1 := byte(v >> 8)
			hash ^= uint64(b1)
			hash *= 1099511628211
			b2 := byte(v >> 16)
			hash ^= uint64(b2)
			hash *= 1099511628211
			b3 := byte(v >> 24)
			hash ^= uint64(b3)
			hash *= 1099511628211

			v = ts[i]
			b0 = byte(v)
			hash ^= uint64(b0)
			hash *= 1099511628211
			b1 = byte(v >> 8)
			hash ^= uint64(b1)
			hash *= 1099511628211
			b2 = byte(v >> 16)
			hash ^= uint64(b2)
			hash *= 1099511628211
			b3 = byte(v >> 24)
			hash ^= uint64(b3)
			hash *= 1099511628211

			lvl := levels[i]
			b0 = byte(lvl)
			hash ^= uint64(b0)
			hash *= 1099511628211
			b1 = byte(lvl >> 8)
			hash ^= uint64(b1)
			hash *= 1099511628211

			ln := lens[i]
			b0 = byte(ln)
			hash ^= uint64(b0)
			hash *= 1099511628211
			b1 = byte(ln >> 8)
			hash ^= uint64(b1)
			hash *= 1099511628211
		}
	}
	return hash
}
