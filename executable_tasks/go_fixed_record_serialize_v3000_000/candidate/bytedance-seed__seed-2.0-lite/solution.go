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
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < n; i++ {
			// Process ids[i] in original little-endian byte order
			id := ids[i]
			h ^= uint64(id & 0xFF)
			h *= fnvPrime
			h ^= uint64((id >> 8) & 0xFF)
			h *= fnvPrime
			h ^= uint64((id >> 16) & 0xFF)
			h *= fnvPrime
			h ^= uint64((id >> 24) & 0xFF)
			h *= fnvPrime

			// Process ts[i]
			t := ts[i]
			h ^= uint64(t & 0xFF)
			h *= fnvPrime
			h ^= uint64((t >> 8) & 0xFF)
			h *= fnvPrime
			h ^= uint64((t >> 16) & 0xFF)
			h *= fnvPrime
			h ^= uint64((t >> 24) & 0xFF)
			h *= fnvPrime

			// Process levels[i]
			lvl := levels[i]
			h ^= uint64(lvl & 0xFF)
			h *= fnvPrime
			h ^= uint64((lvl >> 8) & 0xFF)
			h *= fnvPrime

			// Process lens[i]
			ln := lens[i]
			h ^= uint64(ln & 0xFF)
			h *= fnvPrime
			h ^= uint64((ln >> 8) & 0xFF)
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}