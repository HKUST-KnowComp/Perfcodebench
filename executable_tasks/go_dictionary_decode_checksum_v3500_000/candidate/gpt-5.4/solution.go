package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset64

		// Fuse decode and checksum to avoid the temporary decoded slice and
		// the second full pass over the data.
		i := 0
		n := len(ids)
		for ; i+8 <= n; i += 8 {
			v0 := uint64(dictVals[ids[i]]) + 1
			h ^= v0
			h *= prime64
			v1 := uint64(dictVals[ids[i+1]]) + 1
			h ^= v1
			h *= prime64
			v2 := uint64(dictVals[ids[i+2]]) + 1
			h ^= v2
			h *= prime64
			v3 := uint64(dictVals[ids[i+3]]) + 1
			h ^= v3
			h *= prime64
			v4 := uint64(dictVals[ids[i+4]]) + 1
			h ^= v4
			h *= prime64
			v5 := uint64(dictVals[ids[i+5]]) + 1
			h ^= v5
			h *= prime64
			v6 := uint64(dictVals[ids[i+6]]) + 1
			h ^= v6
			h *= prime64
			v7 := uint64(dictVals[ids[i+7]]) + 1
			h ^= v7
			h *= prime64
		}
		for ; i < n; i++ {
			h ^= uint64(dictVals[ids[i]]) + 1
			h *= prime64
		}
	}
	return h
}
