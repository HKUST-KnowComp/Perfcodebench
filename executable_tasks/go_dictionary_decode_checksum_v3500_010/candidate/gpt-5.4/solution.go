package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset64

		i := 0
		n := len(ids)
		for ; i+8 <= n; i += 8 {
			h ^= uint64(dictVals[ids[i]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+1]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+2]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+3]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+4]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+5]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+6]]) + 1
			h *= prime64
			h ^= uint64(dictVals[ids[i+7]]) + 1
			h *= prime64
		}
		for ; i < n; i++ {
			h ^= uint64(dictVals[ids[i]]) + 1
			h *= prime64
		}
	}
	return h
}
