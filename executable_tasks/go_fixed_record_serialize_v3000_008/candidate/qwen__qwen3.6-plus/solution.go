package main

const (
	fnvInit  = 1469598103934665603
	fnvPrime = 1099511628211
)

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		hash = fnvInit
		for i := 0; i < n; i++ {
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

			l := levels[i]
			hash ^= uint64(byte(l)); hash *= fnvPrime
			hash ^= uint64(byte(l >> 8)); hash *= fnvPrime

			ln := lens[i]
			hash ^= uint64(byte(ln)); hash *= fnvPrime
			hash ^= uint64(byte(ln >> 8)); hash *= fnvPrime
		}
	}
	return hash
}
