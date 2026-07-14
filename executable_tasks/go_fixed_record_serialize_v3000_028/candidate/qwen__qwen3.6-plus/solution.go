package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const fnvPrime = 1099511628211
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := range ids {
			v := ids[i]
			hash ^= uint64(byte(v)); hash *= fnvPrime
			hash ^= uint64(byte(v >> 8)); hash *= fnvPrime
			hash ^= uint64(byte(v >> 16)); hash *= fnvPrime
			hash ^= uint64(byte(v >> 24)); hash *= fnvPrime

			v = ts[i]
			hash ^= uint64(byte(v)); hash *= fnvPrime
			hash ^= uint64(byte(v >> 8)); hash *= fnvPrime
			hash ^= uint64(byte(v >> 16)); hash *= fnvPrime
			hash ^= uint64(byte(v >> 24)); hash *= fnvPrime

			v16 := levels[i]
			hash ^= uint64(byte(v16)); hash *= fnvPrime
			hash ^= uint64(byte(v16 >> 8)); hash *= fnvPrime

			v16 = lens[i]
			hash ^= uint64(byte(v16)); hash *= fnvPrime
			hash ^= uint64(byte(v16 >> 8)); hash *= fnvPrime
		}
	}
	return hash
}