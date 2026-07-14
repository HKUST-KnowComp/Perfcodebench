package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	var h uint64
	
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h = offset
		i := 0
		for n-i >= 4 {
			id0 := ids[i]
			id1 := ids[i+1]
			id2 := ids[i+2]
			id3 := ids[i+3]

			v0 := dictVals[id0]
			v1 := dictVals[id1]
			v2 := dictVals[id2]
			v3 := dictVals[id3]

			h ^= uint64(v0) + 1
			h *= prime
			h ^= uint64(v1) + 1
			h *= prime
			h ^= uint64(v2) + 1
			h *= prime
			h ^= uint64(v3) + 1
			h *= prime

			i += 4
		}
		for ; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= prime
		}
	}
	return h
}
