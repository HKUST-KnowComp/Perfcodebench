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

	var hash uint64
	// Mirror baseline semantics: if iters == 0, return zero
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h := offset
		for i := 0; i < n; i++ {
			// Serialize ids[i] (uint32) little-endian and update hash
			v := ids[i]
			b := byte(v)
			h ^= uint64(b)
			h *= prime
			v >>= 8
			b = byte(v)
			h ^= uint64(b)
			h *= prime
			v >>= 8
			b = byte(v)
			h ^= uint64(b)
			h *= prime
			v >>= 8
			b = byte(v)
			h ^= uint64(b)
			h *= prime

			// Serialize ts[i] (uint32) little-endian and update hash
			v2 := ts[i]
			b = byte(v2)
			h ^= uint64(b)
			h *= prime
			v2 >>= 8
			b = byte(v2)
			h ^= uint64(b)
			h *= prime
			v2 >>= 8
			b = byte(v2)
			h ^= uint64(b)
			h *= prime
			v2 >>= 8
			b = byte(v2)
			h ^= uint64(b)
			h *= prime

			// Serialize levels[i] (uint16) little-endian and update hash
			lv := levels[i]
			b = byte(lv)
			h ^= uint64(b)
			h *= prime
			lv >>= 8
			b = byte(lv)
			h ^= uint64(b)
			h *= prime

			// Serialize lens[i] (uint16) little-endian and update hash
			ln := lens[i]
			b = byte(ln)
			h ^= uint64(b)
			h *= prime
			ln >>= 8
			b = byte(ln)
			h ^= uint64(b)
			h *= prime
		}
		hash = h
	}
	return hash
}
