package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const seed = 1469598103934665603
	const prime = 1099511628211
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = seed
		for _, id := range ids {
			v := uint64(dictVals[id])
			h ^= v + 1
			h *= prime
		}
	}
	return h
}