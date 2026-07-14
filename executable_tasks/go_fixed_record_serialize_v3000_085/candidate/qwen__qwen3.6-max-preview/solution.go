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
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := range ids {
			id := ids[i]
			hash ^= uint64(byte(id)); hash *= fnvPrime
			hash ^= uint64(byte(id >> 8)); hash *= fnvPrime
			hash ^= uint64(byte(id >> 16)); hash *= fnvPrime
			hash ^= uint64(byte(id >> 24)); hash *= fnvPrime

			t := ts[i]
			hash ^= uint64(byte(t)); hash *= fnvPrime
			hash ^= uint64(byte(t >> 8)); hash *= fnvPrime
			hash ^= uint64(byte(t >> 16)); hash *= fnvPrime
			hash ^= uint64(byte(t >> 24)); hash *= fnvPrime

			lv := levels[i]
			hash ^= uint64(byte(lv)); hash *= fnvPrime
			hash ^= uint64(byte(lv >> 8)); hash *= fnvPrime

			ln := lens[i]
			hash ^= uint64(byte(ln)); hash *= fnvPrime
			hash ^= uint64(byte(ln >> 8)); hash *= fnvPrime
		}
	}
	return hash
}