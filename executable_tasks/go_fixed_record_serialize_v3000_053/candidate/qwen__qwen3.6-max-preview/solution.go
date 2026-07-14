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
	n := len(ids)
	const prime uint64 = 1099511628211
	const offset uint64 = 1469598103934665603

	for iter := 0; iter < iters; iter++ {
		hash = offset
		for i := 0; i < n; i++ {
			id := ids[i]
			hash ^= uint64(byte(id)); hash *= prime
			hash ^= uint64(byte(id >> 8)); hash *= prime
			hash ^= uint64(byte(id >> 16)); hash *= prime
			hash ^= uint64(byte(id >> 24)); hash *= prime

			t := ts[i]
			hash ^= uint64(byte(t)); hash *= prime
			hash ^= uint64(byte(t >> 8)); hash *= prime
			hash ^= uint64(byte(t >> 16)); hash *= prime
			hash ^= uint64(byte(t >> 24)); hash *= prime

			lv := levels[i]
			hash ^= uint64(byte(lv)); hash *= prime
			hash ^= uint64(byte(lv >> 8)); hash *= prime

			ln := lens[i]
			hash ^= uint64(byte(ln)); hash *= prime
			hash ^= uint64(byte(ln >> 8)); hash *= prime
		}
	}
	return hash
}