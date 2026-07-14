package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// pre-compute constants
	prime := uint64(1099511628211)
	offset := uint64(1469598103934665603)

	// cache-line aligned scratch buffer reused across iterations
	var buf [256]uint32
	scratch := buf[:]

	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset

		// process 4 ids per inner iteration (unrolled)
		i := 0
		n := len(ids)
		for ; i+3 < n; i += 4 {
			v0 := dictVals[ids[i]]
			v1 := dictVals[ids[i+1]]
			v2 := dictVals[ids[i+2]]
			v3 := dictVals[ids[i+3]]

			h ^= uint64(v0) + 1
			h *= prime
			h ^= uint64(v1) + 1
			h *= prime
			h ^= uint64(v2) + 1
			h *= prime
			h ^= uint64(v3) + 1
			h *= prime
		}

		// tail <4 elements
		for ; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= prime
		}
	}
	return h
}