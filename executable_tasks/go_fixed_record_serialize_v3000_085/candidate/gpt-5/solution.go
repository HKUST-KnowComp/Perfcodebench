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
	const prime uint64 = 1099511628211
	var hash uint64
	
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h := uint64(1469598103934665603)
		for i := 0; i < n; i++ {
			x := ids[i]
			b := byte(x)
			h ^= uint64(b)
			h *= prime
			b = byte(x >> 8)
			h ^= uint64(b)
			h *= prime
			b = byte(x >> 16)
			h ^= uint64(b)
			h *= prime
			b = byte(x >> 24)
			h ^= uint64(b)
			h *= prime

			y := ts[i]
			b = byte(y)
			h ^= uint64(b)
			h *= prime
			b = byte(y >> 8)
			h ^= uint64(b)
			h *= prime
			b = byte(y >> 16)
			h ^= uint64(b)
			h *= prime
			b = byte(y >> 24)
			h ^= uint64(b)
			h *= prime

			lv := levels[i]
			b = byte(lv)
			h ^= uint64(b)
			h *= prime
			b = byte(lv >> 8)
			h ^= uint64(b)
			h *= prime

			ln := lens[i]
			b = byte(ln)
			h ^= uint64(b)
			h *= prime
			b = byte(ln >> 8)
			h ^= uint64(b)
			h *= prime
		}
		hash = h
	}
	return hash
}
