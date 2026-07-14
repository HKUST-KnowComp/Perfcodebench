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
		h := uint64(1469598103934665603)
		for i := 0; i < len(ids); i++ {
			v := ids[i]
			h ^= uint64(byte(v))
			h *= 1099511628211
			h ^= uint64(byte(v >> 8))
			h *= 1099511628211
			h ^= uint64(byte(v >> 16))
			h *= 1099511628211
			h ^= uint64(byte(v >> 24))
			h *= 1099511628211

			v = ts[i]
			h ^= uint64(byte(v))
			h *= 1099511628211
			h ^= uint64(byte(v >> 8))
			h *= 1099511628211
			h ^= uint64(byte(v >> 16))
			h *= 1099511628211
			h ^= uint64(byte(v >> 24))
			h *= 1099511628211

			l := levels[i]
			h ^= uint64(byte(l))
			h *= 1099511628211
			h ^= uint64(byte(l >> 8))
			h *= 1099511628211

			ln := lens[i]
			h ^= uint64(byte(ln))
			h *= 1099511628211
			h ^= uint64(byte(ln >> 8))
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}
