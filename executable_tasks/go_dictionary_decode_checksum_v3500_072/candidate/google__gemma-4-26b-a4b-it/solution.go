package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if len(ids) == 0 {
		return 1469598103934665603
	}

	const offsetBasis = 1469598103934665603
	const prime = 1099511628211

	var h uint64
	n := len(ids)

	for iter := 0; iter < iters; iter++ {
		h = offsetBasis
		i := 0

		// Unroll loop to process 4 elements at a time to improve ILP
		for ; i <= n-4; i += 4 {
			// Element 0
			v0 := dictVals[ids[i]]
			h ^= uint64(v0) + 1
			h *= prime

			// Element 1
			v1 := dictVals[ids[i+1]]
			h ^= uint64(v1) + 1
			h *= prime

			// Element 2
			v2 := dictVals[ids[i+2]]
			h ^= uint64(v2) + 1
			h *= prime

			// Element 3
			v3 := dictVals[ids[i+3]]
			h ^= uint64(v3) + 1
			h *= prime
		}

		// Handle remaining elements
		for ; i < n; i++ {
			h ^= uint64(dictVals[ids[i]]) + 1
			h *= prime
		}
	}

	return h
}