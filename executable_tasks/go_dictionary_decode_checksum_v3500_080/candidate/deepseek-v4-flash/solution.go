package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	const basis = 1469598103934665603
	const prime = 1099511628211
	h := basis
	for _, id := range ids {
		v := dictVals[id]
		h ^= uint64(v) + 1
		h *= prime
	}
	return h
}