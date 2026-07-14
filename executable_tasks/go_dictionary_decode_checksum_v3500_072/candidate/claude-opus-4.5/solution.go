package main

func run(ids, dictVals []uint32, iters int) uint64 {
	n := len(ids)
	if n == 0 {
		var h uint64
		for iter := 0; iter < iters; iter++ {
			h = 1469598103934665603
		}
		return h
	}

	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603

		// Process 8 elements at a time
		i := 0
		n8 := n - 7
		for ; i < n8; i += 8 {
			// Bounds check elimination hints
			_ = ids[i+7]

			v0 := dictVals[ids[i]]
			v1 := dictVals[ids[i+1]]
			v2 := dictVals[ids[i+2]]
			v3 := dictVals[ids[i+3]]
			v4 := dictVals[ids[i+4]]
			v5 := dictVals[ids[i+5]]
			v6 := dictVals[ids[i+6]]
			v7 := dictVals[ids[i+7]]

			h ^= uint64(v0) + 1
			h *= 1099511628211
			h ^= uint64(v1) + 1
			h *= 1099511628211
			h ^= uint64(v2) + 1
			h *= 1099511628211
			h ^= uint64(v3) + 1
			h *= 1099511628211
			h ^= uint64(v4) + 1
			h *= 1099511628211
			h ^= uint64(v5) + 1
			h *= 1099511628211
			h ^= uint64(v6) + 1
			h *= 1099511628211
			h ^= uint64(v7) + 1
			h *= 1099511628211
		}

		// Handle remaining elements
		for ; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= 1099511628211
		}
	}
	return h
}
