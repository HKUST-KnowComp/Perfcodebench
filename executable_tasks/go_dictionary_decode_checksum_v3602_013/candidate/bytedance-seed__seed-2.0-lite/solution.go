package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// Precompute all hash operands once to eliminate per-iteration redundant work
	preVals := make([]uint64, len(ids))
	for i, id := range ids {
		preVals[i] = uint64(dictVals[id]) + 1
	}
	const (
		offset = 1469598103934665603
		prime  = 1099511628211
	)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset
		for _, v := range preVals {
			h ^= v
			h *= prime
		}
	}
	return h
}