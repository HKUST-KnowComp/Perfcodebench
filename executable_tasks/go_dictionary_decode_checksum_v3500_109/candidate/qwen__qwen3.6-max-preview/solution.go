package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	h := uint64(1469598103934665603)
	for _, id := range ids {
		h ^= uint64(dictVals[id]) + 1
		h *= 1099511628211
	}
	return h
}