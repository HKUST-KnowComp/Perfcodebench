package main

const fnvOffset uint64 = 1469598103934665603
const fnvPrime uint64 = 1099511628211

func solutionChecksumBytes(data []byte) uint64 {	// kept for compatibility, not used in optimized path
	var hash uint64 = fnvOffset
	for _, b := range data {
		hash ^= uint64(b)
		hash *= fnvPrime
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < n; i++ {
			v := ids[i]
			// id (uint32) little-endian
			h ^= uint64(byte(v))
			h *= fnvPrime
			h ^= uint64(byte(v >> 8))
			h *= fnvPrime
			h ^= uint64(byte(v >> 16))
			h *= fnvPrime
			h ^= uint64(byte(v >> 24))
			h *= fnvPrime

			v = ts[i]
			// ts (uint32) little-endian
			h ^= uint64(byte(v))
			h *= fnvPrime
			h ^= uint64(byte(v >> 8))
			h *= fnvPrime
			h ^= uint64(byte(v >> 16))
			h *= fnvPrime
			h ^= uint64(byte(v >> 24))
			h *= fnvPrime

			w := levels[i]
			// level (uint16) little-endian
			h ^= uint64(byte(w))
			h *= fnvPrime
			h ^= uint64(byte(w >> 8))
			h *= fnvPrime

			w = lens[i]
			// len (uint16) little-endian
			h ^= uint64(byte(w))
			h *= fnvPrime
			h ^= uint64(byte(w >> 8))
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
