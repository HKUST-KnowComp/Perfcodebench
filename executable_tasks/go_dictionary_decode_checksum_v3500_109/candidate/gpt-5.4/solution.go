package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset64
		for _, id := range ids {
			h ^= uint64(dictVals[id]) + 1
			h *= prime64
		}
	}
	return h
}
