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
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := offset
		if n > 0 {
			_ = ids[n-1]
			_ = ts[n-1]
			_ = levels[n-1]
			_ = lens[n-1]
		}
		for i := 0; i < n; i++ {
			x := ids[i]
			h ^= uint64(byte(x))
			h *= prime
			h ^= uint64(byte(x >> 8))
			h *= prime
			h ^= uint64(byte(x >> 16))
			h *= prime
			h ^= uint64(byte(x >> 24))
			h *= prime

			t := ts[i]
			h ^= uint64(byte(t))
			h *= prime
			h ^= uint64(byte(t >> 8))
			h *= prime
			h ^= uint64(byte(t >> 16))
			h *= prime
			h ^= uint64(byte(t >> 24))
			h *= prime

			lv := levels[i]
			h ^= uint64(byte(lv))
			h *= prime
			h ^= uint64(byte(lv >> 8))
			h *= prime

			ln := lens[i]
			h ^= uint64(byte(ln))
			h *= prime
			h ^= uint64(byte(ln >> 8))
			h *= prime
		}
		hash = h
	}
	return hash
}
