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
		hash = 1469598103934665603
		for i := range ids {
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

			l := levels[i]
			hash ^= uint64(byte(l)); hash *= prime
			hash ^= uint64(byte(l >> 8)); hash *= prime

			n := lens[i]
			hash ^= uint64(byte(n)); hash *= prime
			hash ^= uint64(byte(n >> 8)); hash *= prime
		}
	}
	return hash
}