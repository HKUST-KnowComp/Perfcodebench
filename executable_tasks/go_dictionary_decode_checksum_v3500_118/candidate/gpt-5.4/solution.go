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
		for ; i+4 <= n; i += 4 {
			v0 := dictVals[ids[i]]
			h ^= uint64(v0) + 1
			h *= prime64

			v1 := dictVals[ids[i+1]]
			h ^= uint64(v1) + 1
			h *= prime64

			v2 := dictVals[ids[i+2]]
			h ^= uint64(v2) + 1
			h *= prime64

			v3 := dictVals[ids[i+3]]
			h ^= uint64(v3) + 1
			h *= prime64
		}
		for ; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= prime64
		}
	}
	return h
}
