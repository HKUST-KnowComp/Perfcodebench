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
		hashStart = 1469598103934665603
		hashMul   = 1099511628211
	)
	var finalHash uint64
	for iter := 0; iter < iters; iter++ {
		h := hashStart
		for i := range ids {
			v := ids[i]
			h ^= uint64(byte(v))
			h *= hashMul
			h ^= uint64(byte(v >> 8))
			h *= hashMul
			h ^= uint64(byte(v >> 16))
			h *= hashMul
			h ^= uint64(byte(v >> 24))
			h *= hashMul

			t := ts[i]
			h ^= uint64(byte(t))
			h *= hashMul
			h ^= uint64(byte(t >> 8))
			h *= hashMul
			h ^= uint64(byte(t >> 16))
			h *= hashMul
			h ^= uint64(byte(t >> 24))
			h *= hashMul

			lvl := levels[i]
			h ^= uint64(byte(lvl))
			h *= hashMul
			h ^= uint64(byte(lvl >> 8))
			h *= hashMul

			ln := lens[i]
			h ^= uint64(byte(ln))
			h *= hashMul
			h ^= uint64(byte(ln >> 8))
			h *= hashMul
		}
		finalHash = h
	}
	return finalHash
}