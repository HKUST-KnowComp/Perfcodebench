package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const prime = 1099511628211
	const offset = 1469598103934665603
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset
		for _, id := range ids {
			h ^= uint64(dictVals[id]) + 1
			h *= prime
		}
	}
	return h
}
