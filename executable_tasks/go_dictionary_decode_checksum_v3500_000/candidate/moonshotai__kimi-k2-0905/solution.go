package main

const (
	fnvOffset = 14695981039346656037
	fnvPrime  = 1099511628211
)

func run(ids, dictVals []uint32, iters int) uint64 {
	// No allocation: we stream ids through the hash without a decoded buffer.
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = fnvOffset
		for _, id := range ids {
			h ^= uint64(dictVals[id]) + 1
			h *= fnvPrime
		}
	}
	return h
}