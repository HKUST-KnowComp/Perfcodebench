package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	const (
		offset64 = 1469598103934665603
		prime64  = 1099511628211
	)
	h := uint64(offset64)
	for _, id := range ids {
		h ^= uint64(dictVals[id]) + 1
		h *= prime64
	}
	return h
}
