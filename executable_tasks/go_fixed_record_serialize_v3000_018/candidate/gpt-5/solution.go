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
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < n; i++ {
			v := ids[i]
			h ^= uint64(byte(v))
			h *= fnvPrime
			h ^= uint64(byte(v >> 8))
			h *= fnvPrime
			h ^= uint64(byte(v >> 16))
			h *= fnvPrime
			h ^= uint64(byte(v >> 24))
			h *= fnvPrime

			w := ts[i]
			h ^= uint64(byte(w))
			h *= fnvPrime
			h ^= uint64(byte(w >> 8))
			h *= fnvPrime
			h ^= uint64(byte(w >> 16))
			h *= fnvPrime
			h ^= uint64(byte(w >> 24))
			h *= fnvPrime

			lv := levels[i]
			h ^= uint64(byte(lv))
			h *= fnvPrime
			h ^= uint64(byte(lv >> 8))
			h *= fnvPrime

			ln := lens[i]
			h ^= uint64(byte(ln))
			h *= fnvPrime
			h ^= uint64(byte(ln >> 8))
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
