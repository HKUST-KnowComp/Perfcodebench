package main

func run(ids, dictVals []uint32, iters int) uint64 {
	var h uint64 = 1469598103934665603
	var offsetBasis uint64 = 1469598103934665603
	var prime uint64 = 1099511628211

	for iter := 0; iter < iters; iter++ {
		h = offsetBasis
		for _, id := range ids {
			v := uint64(dictVals[id]) + 1
			h ^= v
			h *= prime
		}
	}
	return h
}