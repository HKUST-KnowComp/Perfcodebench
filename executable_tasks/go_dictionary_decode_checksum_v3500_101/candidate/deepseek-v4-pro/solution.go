package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const (
		basis = 1469598103934665603
		prime = 1099511628211
	)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = basis
		for i := 0; i < len(ids); i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= prime
		}
	}
	return h
}
