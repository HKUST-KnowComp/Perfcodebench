package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	h := offset
	n := len(ids)
	for i := 0; i < n; i++ {
		v := uint64(dictVals[ids[i]]) + 1
		h ^= v
		h *= prime
	}
	return h
}
