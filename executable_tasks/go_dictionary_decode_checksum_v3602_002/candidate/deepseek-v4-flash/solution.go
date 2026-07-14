package main

func run(ids, dictVals []uint32, iters int) uint64 {
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603
		for _, id := range ids {
			d := uint64(dictVals[id]) + 1
			h ^= d
			h *= 1099511628211
		}
	}
	return h
}