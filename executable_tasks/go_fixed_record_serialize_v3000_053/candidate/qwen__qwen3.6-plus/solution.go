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
	const prime = 1099511628211
	const init = 1469598103934665603
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = init
		for i := 0; i < n; i++ {
			v := uint64(ids[i])
			hash ^= v; hash *= prime
			hash ^= v >> 8; hash *= prime
			hash ^= v >> 16; hash *= prime
			hash ^= v >> 24; hash *= prime

			v = uint64(ts[i])
			hash ^= v; hash *= prime
			hash ^= v >> 8; hash *= prime
			hash ^= v >> 16; hash *= prime
			hash ^= v >> 24; hash *= prime

			v = uint64(levels[i])
			hash ^= v; hash *= prime
			hash ^= v >> 8; hash *= prime

			v = uint64(lens[i])
			hash ^= v; hash *= prime
			hash ^= v >> 8; hash *= prime
		}
	}
	return hash
}