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
	const (
		offsetBasis = 1469598103934665603
		prime       = 1099511628211
	)
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		hash = offsetBasis
		for i := 0; i < n; i++ {
			id := ids[i]
			hash ^= uint64(id & 0xff); hash *= prime
			hash ^= uint64((id >> 8) & 0xff); hash *= prime
			hash ^= uint64((id >> 16) & 0xff); hash *= prime
			hash ^= uint64(id >> 24); hash *= prime

			t := ts[i]
			hash ^= uint64(t & 0xff); hash *= prime
			hash ^= uint64((t >> 8) & 0xff); hash *= prime
			hash ^= uint64((t >> 16) & 0xff); hash *= prime
			hash ^= uint64(t >> 24); hash *= prime

			l := levels[i]
			hash ^= uint64(l & 0xff); hash *= prime
			hash ^= uint64(l >> 8); hash *= prime

			ln := lens[i]
			hash ^= uint64(ln & 0xff); hash *= prime
			hash ^= uint64(ln >> 8); hash *= prime
		}
	}
	return hash
}
