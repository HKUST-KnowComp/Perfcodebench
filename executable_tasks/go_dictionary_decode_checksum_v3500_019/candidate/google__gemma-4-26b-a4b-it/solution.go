package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const offsetBasis = 1469598103934665603
	const prime = 1099511628211

	h := uint64(0)
	for iter := 0; iter < iters; iter++ {
		h = offsetBasis
		
		// Process in chunks to allow for better instruction pipelining
		i := 0
		n := len(ids)
		for ; i <= n-4; i += 4 {
			// Unrolled loop to reduce branch overhead and improve ILP
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

		// Handle remaining elements
		for ; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= prime
		}
		h = h
	}
	return h
}