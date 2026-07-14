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
	const prime = 1099511628211
	for iter := 0; iter < iters; iter++ {
		h := uint64(1469598103934665603)
		for i := range ids {
			id := ids[i]
			h ^= uint64(byte(id)); h *= prime
			h ^= uint64(byte(id >> 8)); h *= prime
			h ^= uint64(byte(id >> 16)); h *= prime
			h ^= uint64(byte(id >> 24)); h *= prime

			t := ts[i]
			h ^= uint64(byte(t)); h *= prime
			h ^= uint64(byte(t >> 8)); h *= prime
			h ^= uint64(byte(t >> 16)); h *= prime
			h ^= uint64(byte(t >> 24)); h *= prime

			l := levels[i]
			h ^= uint64(byte(l)); h *= prime
			h ^= uint64(byte(l >> 8)); h *= prime

			ln := lens[i]
			h ^= uint64(byte(ln)); h *= prime
			h ^= uint64(byte(ln >> 8)); h *= prime
		}
		hash = h
	}
	return hash
}