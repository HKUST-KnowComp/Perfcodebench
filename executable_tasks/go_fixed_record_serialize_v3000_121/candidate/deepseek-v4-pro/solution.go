package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	var hash uint64 = 1469598103934665603
	const prime = 1099511628211
	n := len(ids)
	for i := 0; i < n; i++ {
		id := ids[i]
		hash ^= uint64(byte(id))
		hash *= prime
		hash ^= uint64(byte(id >> 8))
		hash *= prime
		hash ^= uint64(byte(id >> 16))
		hash *= prime
		hash ^= uint64(byte(id >> 24))
		hash *= prime

		t := ts[i]
		hash ^= uint64(byte(t))
		hash *= prime
		hash ^= uint64(byte(t >> 8))
		hash *= prime
		hash ^= uint64(byte(t >> 16))
		hash *= prime
		hash ^= uint64(byte(t >> 24))
		hash *= prime

		lvl := levels[i]
		hash ^= uint64(byte(lvl))
		hash *= prime
		hash ^= uint64(byte(lvl >> 8))
		hash *= prime

		ln := lens[i]
		hash ^= uint64(byte(ln))
		hash *= prime
		hash ^= uint64(byte(ln >> 8))
		hash *= prime
	}
	return hash
}
