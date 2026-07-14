package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const h0 = 1469598103934665603
	const prime = 1099511628211
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = h0
		for _, id := range ids {
			v := uint64(dictVals[id]) + 1
			h ^= v
			h *= prime
		}
	}
	return h
}