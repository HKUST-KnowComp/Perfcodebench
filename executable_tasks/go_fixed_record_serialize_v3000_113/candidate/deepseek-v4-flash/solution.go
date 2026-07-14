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
	for i := range ids {
		v := ids[i]
		hash ^= uint64(v & 0xff)
		hash *= 1099511628211
		hash ^= uint64((v >> 8) & 0xff)
		hash *= 1099511628211
		hash ^= uint64((v >> 16) & 0xff)
		hash *= 1099511628211
		hash ^= uint64((v >> 24) & 0xff)
		hash *= 1099511628211

		v = ts[i]
		hash ^= uint64(v & 0xff)
		hash *= 1099511628211
		hash ^= uint64((v >> 8) & 0xff)
		hash *= 1099511628211
		hash ^= uint64((v >> 16) & 0xff)
		hash *= 1099511628211
		hash ^= uint64((v >> 24) & 0xff)
		hash *= 1099511628211

		lv := levels[i]
		hash ^= uint64(lv & 0xff)
		hash *= 1099511628211
		hash ^= uint64((lv >> 8) & 0xff)
		hash *= 1099511628211

		ln := lens[i]
		hash ^= uint64(ln & 0xff)
		hash *= 1099511628211
		hash ^= uint64((ln >> 8) & 0xff)
		hash *= 1099511628211
	}
	_ = iters
	return hash
}
