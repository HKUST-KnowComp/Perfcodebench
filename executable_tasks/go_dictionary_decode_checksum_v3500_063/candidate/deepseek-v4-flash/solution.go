package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const offsetBasis uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offsetBasis
		for _, id := range ids {
			v := dictVals[id]
			h ^= uint64(v) + 1
			h *= prime
		}
	}
	return h
}