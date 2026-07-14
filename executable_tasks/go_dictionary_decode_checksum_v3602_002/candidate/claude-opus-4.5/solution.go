package main

func run(ids, dictVals []uint32, iters int) uint64 {
	var h uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603
		i := 0
		// Process 4 elements at a time
		for ; i+3 < n; i += 4 {
			v0 := dictVals[ids[i]]
			v1 := dictVals[ids[i+1]]
			v2 := dictVals[ids[i+2]]
			v3 := dictVals[ids[i+3]]
			h ^= uint64(v0) + 1
			h *= 1099511628211
			h ^= uint64(v1) + 1
			h *= 1099511628211
			h ^= uint64(v2) + 1
			h *= 1099511628211
			h ^= uint64(v3) + 1
			h *= 1099511628211
		}
		// Handle remaining elements
		for ; i < n; i++ {
			h ^= uint64(dictVals[ids[i]]) + 1
			h *= 1099511628211
		}
	}
	return h
}