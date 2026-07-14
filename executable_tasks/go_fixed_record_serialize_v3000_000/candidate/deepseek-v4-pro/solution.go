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
	const (
		initHash   = 1469598103934665603
		multiplier = 1099511628211
	)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = initHash
		for i := range ids {
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= multiplier
			hash ^= uint64(byte(v >> 8))
			hash *= multiplier
			hash ^= uint64(byte(v >> 16))
			hash *= multiplier
			hash ^= uint64(byte(v >> 24))
			hash *= multiplier

			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= multiplier
			hash ^= uint64(byte(v >> 8))
			hash *= multiplier
			hash ^= uint64(byte(v >> 16))
			hash *= multiplier
			hash ^= uint64(byte(v >> 24))
			hash *= multiplier

			l := levels[i]
			hash ^= uint64(byte(l))
			hash *= multiplier
			hash ^= uint64(byte(l >> 8))
			hash *= multiplier

			l = lens[i]
			hash ^= uint64(byte(l))
			hash *= multiplier
			hash ^= uint64(byte(l >> 8))
			hash *= multiplier
		}
	}
	return hash
}