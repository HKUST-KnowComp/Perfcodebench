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
	const prime = 1099511628211

	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603

		// Process 8 elements at a time
		i := 0
		n8 := n - 7
		for i < n8 {
			id0 := ids[i]
			id1 := ids[i+1]
			id2 := ids[i+2]
			id3 := ids[i+3]
			id4 := ids[i+4]
			id5 := ids[i+5]
			id6 := ids[i+6]
			id7 := ids[i+7]

			v0 := dictVals[id0]
			v1 := dictVals[id1]
			v2 := dictVals[id2]
			v3 := dictVals[id3]
			v4 := dictVals[id4]
			v5 := dictVals[id5]
			v6 := dictVals[id6]
			v7 := dictVals[id7]

			h ^= uint64(v0) + 1
			h *= prime
			h ^= uint64(v1) + 1
			h *= prime
			h ^= uint64(v2) + 1
			h *= prime
			h ^= uint64(v3) + 1
			h *= prime
			h ^= uint64(v4) + 1
			h *= prime
			h ^= uint64(v5) + 1
			h *= prime
			h ^= uint64(v6) + 1
			h *= prime
			h ^= uint64(v7) + 1
			h *= prime

			i += 8
		}

		// Handle remaining elements
		for ; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= prime
		}
	}
	return h
}