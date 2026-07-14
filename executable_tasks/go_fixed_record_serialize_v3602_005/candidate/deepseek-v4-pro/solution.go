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
	var hash uint64 = 1469598103934665603
	n := len(ids)
	for i := 0; i < n; i++ {
		id := ids[i]
		hash ^= uint64(byte(id))
		hash *= 1099511628211
		hash ^= uint64(byte(id >> 8))
		hash *= 1099511628211
		hash ^= uint64(byte(id >> 16))
		hash *= 1099511628211
		hash ^= uint64(byte(id >> 24))
		hash *= 1099511628211

		t := ts[i]
		hash ^= uint64(byte(t))
		hash *= 1099511628211
		hash ^= uint64(byte(t >> 8))
		hash *= 1099511628211
		hash ^= uint64(byte(t >> 16))
		hash *= 1099511628211
		hash ^= uint64(byte(t >> 24))
		hash *= 1099511628211

		lvl := levels[i]
		hash ^= uint64(byte(lvl))
		hash *= 1099511628211
		hash ^= uint64(byte(lvl >> 8))
		hash *= 1099511628211

		ln := lens[i]
		hash ^= uint64(byte(ln))
		hash *= 1099511628211
		hash ^= uint64(byte(ln >> 8))
		hash *= 1099511628211
	}
	return hash
}
