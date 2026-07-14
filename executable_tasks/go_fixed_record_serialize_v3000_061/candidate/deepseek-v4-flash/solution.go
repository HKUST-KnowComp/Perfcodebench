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
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := range ids {
			id := ids[i]
			// id bytes (little-endian)
			hash ^= uint64(id & 0xff)
			hash *= fnvPrime
			hash ^= uint64((id >> 8) & 0xff)
			hash *= fnvPrime
			hash ^= uint64((id >> 16) & 0xff)
			hash *= fnvPrime
			hash ^= uint64((id >> 24) & 0xff)
			hash *= fnvPrime

			t := ts[i]
			// ts bytes (little-endian)
			hash ^= uint64(t & 0xff)
			hash *= fnvPrime
			hash ^= uint64((t >> 8) & 0xff)
			hash *= fnvPrime
			hash ^= uint64((t >> 16) & 0xff)
			hash *= fnvPrime
			hash ^= uint64((t >> 24) & 0xff)
			hash *= fnvPrime

			level := levels[i]
			// level bytes (little-endian)
			hash ^= uint64(level & 0xff)
			hash *= fnvPrime
			hash ^= uint64((level >> 8) & 0xff)
			hash *= fnvPrime

			l := lens[i]
			// lens bytes (little-endian)
			hash ^= uint64(l & 0xff)
			hash *= fnvPrime
			hash ^= uint64((l >> 8) & 0xff)
			hash *= fnvPrime
		}
	}
	return hash
}
