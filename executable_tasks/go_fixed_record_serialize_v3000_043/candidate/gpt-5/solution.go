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
	n := len(ids)
	// Assume all slices are of equal length as per task definition
	for iter := 0; iter < iters; iter++ {
		h := offset

		idsLocal := ids
		tsLocal := ts
		levelsLocal := levels
		lensLocal := lens

		for i := 0; i < n; i++ {
			v := idsLocal[i]
			h ^= uint64(byte(v))
			h *= prime
			h ^= uint64(byte(v >> 8))
			h *= prime
			h ^= uint64(byte(v >> 16))
			h *= prime
			h ^= uint64(byte(v >> 24))
			h *= prime

			t := tsLocal[i]
			h ^= uint64(byte(t))
			h *= prime
			h ^= uint64(byte(t >> 8))
			h *= prime
			h ^= uint64(byte(t >> 16))
			h *= prime
			h ^= uint64(byte(t >> 24))
			h *= prime

			lv := levelsLocal[i]
			h ^= uint64(byte(lv))
			h *= prime
			h ^= uint64(byte(lv >> 8))
			h *= prime

			ln := lensLocal[i]
			h ^= uint64(byte(ln))
			h *= prime
			h ^= uint64(byte(ln >> 8))
			h *= prime
		}
		hash = h
	}
	return hash
}
