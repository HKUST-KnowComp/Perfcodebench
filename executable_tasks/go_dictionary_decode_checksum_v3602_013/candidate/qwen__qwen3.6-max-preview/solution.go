package main

func run(ids, dictVals []uint32, iters int) uint64 {
	var h uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603
		for i := 0; i < n; i++ {
			h ^= uint64(dictVals[ids[i]]) + 1
			h *= 1099511628211
		}
	}
	return h
}