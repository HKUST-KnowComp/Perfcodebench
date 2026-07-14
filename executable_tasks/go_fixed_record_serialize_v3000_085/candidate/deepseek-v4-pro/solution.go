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
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < n; i++ {
			// ids[i]
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= 1099511628211
			hash ^= uint64(byte(v >> 8))
			hash *= 1099511628211
			hash ^= uint64(byte(v >> 16))
			hash *= 1099511628211
			hash ^= uint64(byte(v >> 24))
			hash *= 1099511628211
			// ts[i]
			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= 1099511628211
			hash ^= uint64(byte(v >> 8))
			hash *= 1099511628211
			hash ^= uint64(byte(v >> 16))
			hash *= 1099511628211
			hash ^= uint64(byte(v >> 24))
			hash *= 1099511628211
			// levels[i]
			lv := levels[i]
			hash ^= uint64(byte(lv))
			hash *= 1099511628211
			hash ^= uint64(byte(lv >> 8))
			hash *= 1099511628211
			// lens[i]
			lv = lens[i]
			hash ^= uint64(byte(lv))
			hash *= 1099511628211
			hash ^= uint64(byte(lv >> 8))
			hash *= 1099511628211
		}
	}
	return hash
}