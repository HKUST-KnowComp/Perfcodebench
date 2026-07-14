package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const seed uint64 = 1469598103934665603
	const mul uint64 = 1099511628211

	var h uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h = seed
		for i := 0; i < n; i++ {
			v := dictVals[ids[i]]
			h ^= uint64(v) + 1
			h *= mul
		}
	}
	return h
}