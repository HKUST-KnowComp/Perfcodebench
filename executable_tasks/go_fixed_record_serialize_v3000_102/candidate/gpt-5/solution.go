package main

const fnvOffset uint64 = 1469598103934665603
const fnvPrime uint64 = 1099511628211

func solutionChecksumBytes(data []byte) uint64 {
	// FNV-1a over data
	h := fnvOffset
	for i := 0; i < len(data); i++ {
		h ^= uint64(data[i])
		h *= fnvPrime
	}
	return h
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	n := len(ids)
	// Bounds-check elimination for other slices used with index i
	if n > 0 {
		_ = ts[n-1]
		_ = levels[n-1]
		_ = lens[n-1]
	}
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < n; i++ {
			id := ids[i]
			t := ts[i]
			lv := levels[i]
			ln := lens[i]

			// id uint32 little-endian
			h ^= uint64(byte(id))
			h *= fnvPrime
			h ^= uint64(byte(id >> 8))
			h *= fnvPrime
			h ^= uint64(byte(id >> 16))
			h *= fnvPrime
			h ^= uint64(byte(id >> 24))
			h *= fnvPrime

			// ts uint32 little-endian
			h ^= uint64(byte(t))
			h *= fnvPrime
			h ^= uint64(byte(t >> 8))
			h *= fnvPrime
			h ^= uint64(byte(t >> 16))
			h *= fnvPrime
			h ^= uint64(byte(t >> 24))
			h *= fnvPrime

			// levels uint16 little-endian
			h ^= uint64(byte(lv))
			h *= fnvPrime
			h ^= uint64(byte(lv >> 8))
			h *= fnvPrime

			// lens uint16 little-endian
			h ^= uint64(byte(ln))
			h *= fnvPrime
			h ^= uint64(byte(ln >> 8))
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
